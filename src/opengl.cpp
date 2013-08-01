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
const double near = 1;
const double far = 128.0;

pipeline* pipel;
camera* cam;

int delta = 0;
bool running = true;

int mouse_x = 0;
int mouse_y = 0;

fbo* geometry;
fbo* depth;

shader* texture;
shader* geometry_shader;

unsigned int mesh;

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
	
	geometry = new fbo(width, height);
	depth = new fbo(width*2, height*2);
	
	texture = new shader("shaders/texture.vert", "shaders/texture.frag");
	geometry_shader = new shader("shaders/geometry.vert", "shaders/geometry.frag");
}

float a = 0.0f;
void logic () {
	cam->update();
	a += delta * 0.00025f;
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
	glCallList(mesh);
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
	pipel->rotateX(25);
	pipel->rotateY(-45);
	pipel->translate(-15, -15, -15);
	glViewport(0, 0, width*2, height*2);
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
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pipel->matrix_mode(PROJECTION_MATRIX);
	pipel->load_identity();
	pipel->ortho(-1, 1, -1, 1, -1, 1);
	pipel->matrix_mode(VIEW_MATRIX);
	pipel->load_identity();
	glViewport(0, 0, width, height);
	pipel->matrix_mode(MODEL_MATRIX);
	pipel->load_identity();
	
	glColor3f(1,1,1);
	glUseProgram(texture->get_program());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, geometry->get_texture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depth->get_depth_texture());
	glUniform1i(glGetUniformLocation(texture->get_program(), "texture"), 0);
	glUniform1i(glGetUniformLocation(texture->get_program(), "texture2"), 1);
	glUniform2f(glGetUniformLocation(texture->get_program(), "resolution"), width, height);
	glUniform1f(glGetUniformLocation(texture->get_program(), "time"), float(SDL_GetTicks())/1000.);
	pipel->update_glsl(texture->get_program());
	glBegin(GL_QUADS);
		glVertex2f(-1, -1);
		glVertex2f(-1, 1);
		glVertex2f(1, 1);
		glVertex2f(1, -1);
	glEnd();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glEnd();
}

void cleanUp () {
	delete geometry;
	delete texture;
	delete geometry_shader;
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
