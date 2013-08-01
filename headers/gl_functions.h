#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <iostream>
#include <SDL/SDL_image.h>
#include "SOIL.h"
#include "math_3d.h"
#include "mesh_loader.h"

#ifndef GL_FUNCTIONS_H
#define GL_FUNCTIONS_H

struct ray {															// simple structure that stores a ray in a nice and clean-looking way
	Vector3f o;															// random point of the ray (normally the origin)
	Vector3f d;															// direction of the ray (a directional vector, basically)
	
	ray () {
	}
	
	ray (Vector3f _o, Vector3f _d) {
		o = _o;
		d = _d;
	}
};

void gl_perspective_projection (float, float, float, float, float);
void gl_ortho_projection (float, float, float, float, float, float);
void gl_circle (float, float, float, bool);
void gl_terrace_corner(float);
void gl_terrace_strai(float);
ray gl_ray (int, int);													// here we asume that upper-left being the (0,0)
unsigned int create_texture (int, int , bool);
unsigned int load_texture (const char*, unsigned int, unsigned int);
unsigned int bake_mesh (mesh_loader);

#endif
