#include <iostream>
#include "../headers/heightmap.h"
using namespace std;

void heightmap::load (const char* file) {
	SDL_Surface* img = SDL_LoadBMP(file);
	//if (not img) throw 1;	// throw an error exception (experimental)
	matrix.resize(img->h, VFh(img->w, 0.0));
	normals.resize(img->h, V3h(img->w, Vector3f(0, 0, 0)));
	//std::cerr << img->w << "x" << img->w << std::endl;
	
	for (int i = 0; i < img->h; ++i)
		for (int x = 0; x < img->w; ++x) {
			Uint32 pixel = ((Uint32*)img->pixels)[i*img->pitch/4 + x];
			Uint8 r,g,b;
			SDL_GetRGB(pixel, img->format, &r, &g, &b);
			matrix[i][x] = 16.0 * (float) r / 255.0;
		}
	SDL_FreeSurface(img);
	
	// compute normals for every vertex (using the cross product)
	for (int i = 0; i < matrix.size()-1; ++i)
		for (int x = 0; x < matrix[0].size()-1; ++x) {
			float ax = x;
			float az = i;
			float ay = matrix[i][x];
			
			float bx = x+1;
			float bz = i;
			float by = matrix[i][x+1];
			
			float cx = x;
			float cz = i+1;
			float cy = matrix[i+1][x];
			
			// U = B - A
			float ux = bx-ax;
			float uy = by-ay;
			float uz = bz-az;
			
			// V = C - A
			float vx = cx-ax;
			float vy = cy-ay;
			float vz = cz-az;
			
			// N = U x V
			float nx = uy*vz - uz*vy;
			float ny = uz*vx - ux*vz;
			float nz = ux*vy - uy*vx;
			
			normals[i][x] += Vector3f(nx, -ny, nz);
		}
}

void heightmap::gl_render () {
	for (int i = 0; i < matrix[0].size()-1; ++i) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < matrix.size(); ++x) {
			glNormal3f(0, 1, 0);	glVertex3f(x, matrix[x][i], i);
			glNormal3f(0, 1, 0);	glVertex3f(x, matrix[x][i+1], i+1);
		}
		glEnd();
	}
}
