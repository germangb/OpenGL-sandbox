#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <SDL/SDL.h>
#include <GLee.h>
#include <cmath>
#include "../headers/gl_functions.h"
#include "../headers/fbo.h"
#include "../headers/shader.h"
#include "../headers/camera.h"
#include "../headers/mesh_loader.h"
#include "../headers/pipeline.h"

int width = 950;
int height = 550;
const char* title = "curves";
const int fov = 40;
const int fps = 60;
const float near = 1.0f;
const float far = 128.0f;

pipeline* pipel;
camera* cam;

int delta = 0;
bool running = true;

int mouse_x = 0;
int mouse_y = 0;

fbo* geometry;
fbo* depth;
fbo* blur_ping;
fbo* blur_pong;

shader* combine_shader;
shader* geometry_shader;
shader* simple_shader;
shader* blur_shader;
shader* texture_shader;

float lightmap_res_cont = 1.f;
unsigned int mesh;
unsigned int texture;

void init () {
	pipel = new pipeline();
	cam = new camera(pipel);
	
	cam->set_viewport(width, height);
	cam->set_fov(fov);
	cam->set_aspect_ratio(float(width)/float(height));
	cam->set_near(near);
	cam->set_far(far);
	
	cam->set_animate(false);
	cam->set_x(-3);
	cam->set_y(45);
	cam->set_z(-3);
	cam->set_rx(75);
	cam->set_ry(30);
	cam->set_animate(true);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	mesh = bake_mesh(mesh_loader("meshes/modul.obj"));
	texture = load_texture("gfx/stairs_uv.png", GL_LINEAR, GL_REPEAT);
	
	geometry = new fbo(width, height);
	blur_ping = new fbo(width, height);
	blur_pong = new fbo(width, height);
	depth = new fbo(width*lightmap_res_cont, height*lightmap_res_cont);
	
	combine_shader = new shader("shaders/texture.vert", "shaders/combine.frag");
	texture_shader = new shader("shaders/texture.vert", "shaders/texture.frag");
	geometry_shader = new shader("shaders/geometry.vert", "shaders/geometry.frag");
	simple_shader = new shader("shaders/simple.vert", "shaders/simple.frag");
	blur_shader = new shader("shaders/texture.vert", "shaders/blur.frag");
}

void gaussian_blur (fbo* a, fbo* b, int passes) {
	for (int i = 0; i < passes*2; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, (i%2==0)?b->get_fbo_id():a->get_fbo_id());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (i%2==0)?a->get_texture():b->get_texture(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pipel->matrix_mode(PROJECTION_MATRIX);
		pipel->load_identity();
		pipel->ortho(0, width, 0, height, -1, 1);
		pipel->matrix_mode(VIEW_MATRIX);
		pipel->load_identity();
		glViewport(0, 0, width, height);
		pipel->matrix_mode(MODEL_MATRIX);
		pipel->load_identity();
		
		glUseProgram(blur_shader->get_program());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (i%2==0)?b->get_texture():a->get_texture());
		glUniform1i(glGetUniformLocation(blur_shader->get_program(), "texture"), 0);
		glUniform2f(glGetUniformLocation(blur_shader->get_program(), "resolution"), width, height);
		glUniform1i(glGetUniformLocation(blur_shader->get_program(), "is_vert"), i%2);
		pipel->update_glsl(blur_shader->get_program());
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(width, 0);
			glVertex2f(width, height);
			glVertex2f(0, height);
		glEnd();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
}

void logic () {
	cam->update();
}

void render_scene (unsigned int program_id) {
	glColor3f(127./255., 120./255., 100./255.);
	glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	glVertex3f(-50, 0, -50);
		glNormal3f(0, 1, 0);	glVertex3f(-50, 0, 50);
		glNormal3f(0, 1, 0);	glVertex3f(50, 0, 50);
		glNormal3f(0, 1, 0);	glVertex3f(50, 0, -50);
	glEnd();
	pipel->push_matrix();
	pipel->translate(5, 0, -20);
	pipel->update_glsl(program_id);
	glColor3f(119./255., 101./255., 88./255.);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program_id, "texture"), 0);
	glCallList(mesh);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	pipel->pop_matrix();
	glColor3f(1,1,1);
}

glm::mat4 shadow_view;
glm::mat4 shadow_projection;
glm::mat4 fix_coord(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

void get_depth_map () {
	glBindFramebuffer(GL_FRAMEBUFFER, depth->get_fbo_id());
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pipel->matrix_mode(PROJECTION_MATRIX);
	pipel->load_identity();
	pipel->ortho(-width/25, width/25, -height/25, height/25, -100, 100);
	pipel->matrix_mode(VIEW_MATRIX);
	pipel->load_identity();
	pipel->rotateX(45);
	pipel->rotateY(-45);
	pipel->translate(-3, -25, -3);
	glViewport(0, 0, width*lightmap_res_cont, height*lightmap_res_cont);
	pipel->matrix_mode(MODEL_MATRIX);
	pipel->load_identity();
	
	shadow_view = pipel->get_view_matrix();
	shadow_projection = pipel->get_projection_matrix();
	
	glUseProgram(geometry_shader->get_program());
	
	pipel->update_glsl(geometry_shader->get_program());

	render_scene(geometry_shader->get_program());
	glUseProgram(0);
}

void render () {
	get_depth_map();
	
	glBindFramebuffer(GL_FRAMEBUFFER, geometry->get_fbo_id());
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cam->use();
	pipel->matrix_mode(MODEL_MATRIX);
	pipel->load_identity();
	glUseProgram(geometry_shader->get_program());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depth->get_depth_texture());
	glUniform1i(glGetUniformLocation(geometry_shader->get_program(), "shadow_map"), 1);
	pipel->update_glsl(geometry_shader->get_program());
	glUniformMatrix4fv(glGetUniformLocation(geometry_shader->get_program(), "shadow_view"), 1, GL_FALSE, &shadow_view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(geometry_shader->get_program(), "shadow_projection"), 1, GL_FALSE, &shadow_projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(geometry_shader->get_program(), "shadow_fix"), 1, GL_FALSE, &fix_coord[0][0]);
	render_scene(geometry_shader->get_program());
	glUseProgram(0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, blur_ping->get_fbo_id());
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pipel->matrix_mode(PROJECTION_MATRIX);
	pipel->load_identity();
	pipel->ortho(0, width, 0, height, -1, 1);
	pipel->matrix_mode(VIEW_MATRIX);
	pipel->load_identity();
	glViewport(0, 0, width, height);
	pipel->matrix_mode(MODEL_MATRIX);
	pipel->load_identity();
	glUseProgram(texture_shader->get_program());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, geometry->get_texture());
	glUniform1i(glGetUniformLocation(texture_shader->get_program(), "texture"), 0);
	glUniform2f(glGetUniformLocation(texture_shader->get_program(), "resolution"), width, height);
	pipel->update_glsl(texture_shader->get_program());
	glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(width, 0);
		glVertex2f(width, height);
		glVertex2f(0, height);
	glEnd();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	
	gaussian_blur(blur_ping, blur_pong, 2);
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pipel->matrix_mode(PROJECTION_MATRIX);
	pipel->load_identity();
	pipel->ortho(0, width, 0, height, -1, 1);
	pipel->matrix_mode(VIEW_MATRIX);
	pipel->load_identity();
	glViewport(0, 0, width, height);
	pipel->matrix_mode(MODEL_MATRIX);
	pipel->load_identity();
	
	glColor3f(1,1,1);
	glUseProgram(combine_shader->get_program());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, geometry->get_texture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, geometry->get_depth_texture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blur_pong->get_texture());
	glUniform1i(glGetUniformLocation(combine_shader->get_program(), "composite"), 0);
	glUniform1i(glGetUniformLocation(combine_shader->get_program(), "gdepth"), 1);
	glUniform1i(glGetUniformLocation(combine_shader->get_program(), "blurred"), 2);
	glUniform2f(glGetUniformLocation(combine_shader->get_program(), "resolution"), width, height);
	glUniform1f(glGetUniformLocation(combine_shader->get_program(), "time"), float(SDL_GetTicks())/1000.);
	pipel->update_glsl(combine_shader->get_program());
	glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(width, 0);
		glVertex2f(width, height);
		glVertex2f(0, height);
	glEnd();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
	/*
	pipel->load_identity();
	glUseProgram(simple_shader->get_program());
	pipel->push_matrix();
	pipel->translate(0, 0, 0);
	pipel->update_glsl(simple_shader->get_program());
	glBegin(GL_QUADS);
		glColor4f(0,0,0,.35f);
		glVertex2f(0, 0);
		glVertex2f(0, 42);
		glColor4f(0,0,0,0);
		glVertex2f(256, 42);
		glVertex2f(256, 0);
	glEnd();
	glColor4f(1,1,1,1);
	pipel->pop_matrix();
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);*/
}

void cleanUp () {
	SDL_Quit();
}

int main (int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
	SDL_WM_SetCaption(title, title);
	SDL_Event event;
	Uint32 start;
	init();
	
	while (running) {
		start = SDL_GetTicks();
		while (SDL_PollEvent(&event))
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEMOTION:
					mouse_x = event.button.x;
					mouse_y = event.button.y;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_LEFT:
							break;
						case SDLK_RIGHT:
							break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym) {
						case SDLK_p:
							running = false;
							break;
						case SDLK_LEFT:
							break;
					}
					break;
			}
		logic();
		render();
		SDL_GL_SwapBuffers();
		if (1000/fps > SDL_GetTicks() - start)
			SDL_Delay(1000/fps - (SDL_GetTicks() - start));
		delta = start - SDL_GetTicks();
	}
	
	cleanUp();
	return 0;
}
