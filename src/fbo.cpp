#include "../headers/fbo.h"

fbo::fbo () {
	
}

fbo::fbo (int w, int h) {
	load(w, h);
}

void fbo::load (int width, int height) {
	color_texture_id = create_texture(width, height, false);
	depth_texture_id = create_texture(width, height, true);
	glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);
	
	glBindTexture(GL_TEXTURE_2D, color_texture_id);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	int err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "error while creating fbo: " << gluErrorString(err) << "\n";
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int fbo::get_fbo_id () {
	return fbo_id;
}

unsigned int fbo::get_texture () {
	return color_texture_id;
}

unsigned int fbo::get_depth_texture () {
	return depth_texture_id;
}
