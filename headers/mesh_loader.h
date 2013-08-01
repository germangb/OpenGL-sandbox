#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "math_3d.h"
#ifndef MESH_H
#define MESH_H

struct face {
	int v1;
	int v2;
	int v3;
	
	int n1;
	int n2;
	int n3;
	
	int uv1;
	int uv2;
	int uv3;
};

typedef std::vector<face> VF;
typedef std::vector<Vector3f> VV3;
typedef std::vector<Vector2f> VV2;
typedef std::stringstream SS;

class mesh_loader {
	private:
		VV3 vertex;
		VV3 normals;
		VV2 uv;
		VF faces;
		unsigned int texture;
		
	public:
		mesh_loader(const char*);
		VF& get_faces ();
		VV3& get_vertex ();
		VV3& get_normals ();
		VV2& get_uvs ();
		unsigned int get_texture ();
};

#endif
