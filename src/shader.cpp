#include "../headers/shader.h"

shader::shader () {
	
}

shader::shader(const char* v, const char* f) {
	init_shaders(v, f);
}

shader::~shader () {
	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);
}

void shader::init_shaders (const char* vs_f, const char* fs_f) {
	load_shader(vs_f, GL_VERTEX_SHADER, vs);
	load_shader(fs_f, GL_FRAGMENT_SHADER, fs);
	std::cerr << "----------------\n";
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
}

void shader::load_shader (const char* f, unsigned int mode, unsigned int& id) {
	std::string source = "";
	std::string line;
	std::ifstream fs(f);
	id = glCreateShader(mode);

	if (not fs.is_open()) {
		std::cerr << "the shader <" << f << "> couln't be opened" << "\n";
		return;
	}
	
	while (not fs.eof()) {
		getline(fs, line);
		source += line+"\n";
	}
	
	fs.close();
	
	const char* csource=source.c_str();
	glShaderSource(id, 1, &csource, NULL);
	glCompileShader(id);
	char error[1000];
	glGetShaderInfoLog(id,1000,NULL,error);
	std::cerr << "compile log:\n" << error << "\n"; 
}

unsigned int shader::get_program() {
	return program;
}
