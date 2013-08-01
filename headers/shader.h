#include <GLee.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <string>
#include <fstream>
#ifndef SHADER_H
#define SHADER_H

class shader {
	private:
		unsigned int program;
		unsigned int vs;
		unsigned int fs;
	public:
		shader();
		shader(const char*, const char*);
		~shader();
		void load_shader(const char*, unsigned int, unsigned int&);
		void init_shaders (const char*, const char*);
		unsigned int get_program();
};

#endif
