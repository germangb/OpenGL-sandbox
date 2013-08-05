#include <vector>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "math_3d.h"
#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

typedef std::vector<float> VFh;
typedef std::vector<VFh> VVFh;
typedef std::vector<Vector3f> V3h;
typedef std::vector<V3h> VV3h;

class heightmap {
	private:
		VVFh matrix;
		VV3h normals;
	public:
		void load (const char*);
		void gl_render();
};

#endif
