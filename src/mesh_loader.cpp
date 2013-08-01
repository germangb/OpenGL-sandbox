#include "../headers/mesh_loader.h"
#include <cstdlib>
#include <cstdio>

mesh_loader::mesh_loader (const char* file) {
	std::ifstream fs(file);
	std::string line;
	
	while (!fs.eof()) {
		getline(fs, line);
		
		if (line[0] == 'v' and line[1] != 'n' and line[1] != 't') {
			float x, y, z;
			sscanf(line.c_str(),"v %f %f %f", &x, &y, &z);
			vertex.push_back(Vector3f(x, y, z));
			//std::cerr << "v " << x << " " << y << " " << z << "\n";
		} else if (line[0] == 'v' and line[1] == 'n') {
			float x, y, z;
			sscanf(line.c_str(),"vn %f %f %f", &x, &y, &z);
			normals.push_back(Vector3f(x, y, z));
			//std::cerr << "vn " << x << " " << y << " " << z << "\n";
		} else if (line[0] == 'v' and line[1] == 't') {
			float x, y;
			sscanf(line.c_str(),"vt %f %f", &x, &y);
			uv.push_back(Vector2f(x, y));
			//std::cerr << "vn " << x << " " << y << " " << z << "\n";
		} else if (line[0] == 'f') {
			face tmp;
			//std::cerr << line << std::endl;
			sscanf(line.c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d", &(tmp.v1), &(tmp.uv1), &(tmp.n1), &(tmp.v2), &(tmp.uv2), &(tmp.n2), &(tmp.v3), &(tmp.uv3), &(tmp.n3));
			faces.push_back(tmp);
			//std::cerr << "f " << tmp.v1 << "/" << tmp.uv1 << "/" << tmp.n1 << " " << tmp.v2 << "/" << tmp.uv2 << "/" << tmp.n2 << " "<< tmp.v3 << "/" << tmp.uv3 << "/" << tmp.n3 << "\n";
		}
	}
	
	fs.close();
}

VF& mesh_loader::get_faces () {
	return faces;
}

VV3& mesh_loader::get_vertex () {
	return vertex;
}

VV3& mesh_loader::get_normals () {
	return normals;
}

VV2& mesh_loader::get_uvs () {
	return uv;
}
