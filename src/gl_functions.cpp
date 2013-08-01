#include "../headers/gl_functions.h"

void gl_perspective_projection (float fov, float w, float h, float near, float far) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, w/h, near, far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
}

void gl_ortho_projection (float left, float right, float bottom, float top, float near, float far) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, near, far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, right-left, top-bottom);
	glDisable(GL_DEPTH_TEST);
}

void gl_circle (float radius, float x, float y, bool line) {
	if (not line) {
		glBegin(GL_TRIANGLE_FAN);
			glVertex3f(0, 0, 0);
			for (int i = 0; i <= 360; i += 20)
				glVertex3f(x+cos(float(i)*M_PI/180.0)*radius, 0, y+sin(float(i)*M_PI/180.0)*radius);
		glEnd();
	} else {
		glBegin(GL_LINE_LOOP);
			for (int i = 0; i < 360; i += 20)
				glVertex3f(x+cos(float(i)*M_PI/180.0)*radius, 0, y+sin(float(i)*M_PI/180.0)*radius);
		glEnd();
	}
}

void gl_terrace_strai (float w) {
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(0, 0, 0);
		glVertex3f(w, 0, 0);
		glVertex3f(0, 5, 20);
		glVertex3f(w, 5, 20);
		glVertex3f(0, 5, 25);
		glVertex3f(w, 5, 25);
		glVertex3f(0, 0, 25);
		glVertex3f(w, 0, 25);
		glVertex3f(0, 0, 0);
		glVertex3f(w, 0, 0);
	glEnd();
	
	glBegin(GL_QUADS);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 25);
		glVertex3f(0, 5, 25);
		glVertex3f(0, 5, 20);
		
		glVertex3f(w, 0, 0);
		glVertex3f(w, 0, 25);
		glVertex3f(w, 5, 25);
		glVertex3f(w, 5, 20);
	glEnd();
}

void gl_terrace_corner(float rad) {
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= 90; i += 30) {
		glVertex3f(cos(float(i)*M_PI/180.)*rad, 0, sin(float(i)*M_PI/180.)*rad);
		glVertex3f(cos(float(i)*M_PI/180.)*(rad+20), 5, sin(float(i)*M_PI/180.)*(rad+20));
	}
	glEnd();
	
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= 90; i += 30) {
		glVertex3f(cos(float(i)*M_PI/180.)*(rad+20), 5, sin(float(i)*M_PI/180.)*(rad+20));
		glVertex3f(cos(float(i)*M_PI/180.)*(rad+25), 5, sin(float(i)*M_PI/180.)*(rad+25));
	}
	glEnd();
	
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= 90; i += 30) {
		glVertex3f(cos(float(i)*M_PI/180.)*(rad+25), 5, sin(float(i)*M_PI/180.)*(rad+25));
		glVertex3f(cos(float(i)*M_PI/180.)*(rad+25), 0, sin(float(i)*M_PI/180.)*(rad+25));
	}
	glEnd();
	
	glBegin(GL_QUADS);
		glVertex3f(rad,0,0);
		glVertex3f(rad+20,5,0);
		glVertex3f(rad+25,5,0);
		glVertex3f(rad+25,0,0);
		
		glVertex3f(0,0,rad);
		glVertex3f(0,5,rad+20);
		glVertex3f(0,5,rad+25);
		glVertex3f(0,0,rad+25);
	glEnd();
}

ray gl_ray (int cursor_x, int cursor_y) {
	// this code was provided by @sler from stackoverflow
	// http://stackoverflow.com/questions/9406269/object-picking-with-ray-casting
	
	GLint viewport[4];
	GLdouble modelMatrix[16];
	GLdouble projectionMatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

	GLfloat winY = GLfloat(viewport[3] - cursor_y);

	double ox, oy, oz;
	gluUnProject( (double) cursor_x, winY, 0.0f, modelMatrix, projectionMatrix, viewport, &ox, &oy, &oz );
	double fx, fy, fz;
	gluUnProject( (double) cursor_x, winY, 1.0f, modelMatrix, projectionMatrix, viewport, &fx, &fy, &fz );
		
	double vx = fx-ox;
	double vy = fy-oy;
	double vz = fz-oz;
	
	return ray(Vector3f((float)ox, (float)oy, (float)oz), Vector3f((float)vx, (float)vy, (float)vz));
}

unsigned int create_texture (int w, int h, bool is_depth) {
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, (is_depth ? GL_DEPTH_COMPONENT : GL_RGBA8), w, h, 0, (is_depth ? GL_DEPTH_COMPONENT : GL_RGBA), GL_FLOAT,  NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	int err = glGetError();
	if (err != 0)
		std::cerr << "error while creating texture: " << gluErrorString(err) << std::endl;
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return texture_id;
}

unsigned int load_texture (const char* file, unsigned int filter, unsigned int wrap) {
	// this piece of code was inspired (sounds better than "copied") by
	// http://open.gl/textures
	unsigned int id;
	int width;
	int height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGBA);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glBindTexture(GL_TEXTURE_2D, 0);
	return id;
}

unsigned int bake_mesh (mesh_loader m) {
	unsigned int id = glGenLists(1);
	
	glNewList(id, GL_COMPILE);
	glBegin(GL_TRIANGLES);
		for (int i = 0; i < int(m.get_faces().size()); ++i) {
			//std::cerr << m.get_uvs()[m.get_faces()[i].uv1-1].x << " " << m.get_uvs()[m.get_faces()[i].uv1-1].y << std::endl;
			glTexCoord2f(m.get_uvs()[m.get_faces()[i].uv1-1].x, 1.0f-m.get_uvs()[m.get_faces()[i].uv1-1].y);
			glNormal3f(m.get_normals()[m.get_faces()[i].n1-1].x, m.get_normals()[m.get_faces()[i].n1-1].y, m.get_normals()[m.get_faces()[i].n1-1].z);
			glVertex3f(m.get_vertex()[m.get_faces()[i].v1-1].x, m.get_vertex()[m.get_faces()[i].v1-1].y, m.get_vertex()[m.get_faces()[i].v1-1].z);
			
			glTexCoord2f(m.get_uvs()[m.get_faces()[i].uv2-1].x, 1.0f-m.get_uvs()[m.get_faces()[i].uv2-1].y);
			glNormal3f(m.get_normals()[m.get_faces()[i].n2-1].x, m.get_normals()[m.get_faces()[i].n2-1].y, m.get_normals()[m.get_faces()[i].n2-1].z);
			glVertex3f(m.get_vertex()[m.get_faces()[i].v2-1].x, m.get_vertex()[m.get_faces()[i].v2-1].y, m.get_vertex()[m.get_faces()[i].v2-1].z);
			
			glTexCoord2f(m.get_uvs()[m.get_faces()[i].uv3-1].x, 1.0f-m.get_uvs()[m.get_faces()[i].uv3-1].y);
			glNormal3f(m.get_normals()[m.get_faces()[i].n3-1].x, m.get_normals()[m.get_faces()[i].n3-1].y, m.get_normals()[m.get_faces()[i].n3-1].z);
			glVertex3f(m.get_vertex()[m.get_faces()[i].v3-1].x, m.get_vertex()[m.get_faces()[i].v3-1].y, m.get_vertex()[m.get_faces()[i].v3-1].z);
		}
	glEnd();
	glEndList();
	
	return id;
}
