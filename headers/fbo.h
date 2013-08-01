#include <iostream>
#include <GLee.h>
#include "gl_functions.h"
#ifndef FBO_H
#define FBO_H

class fbo {
	private:
		unsigned int fbo_id;
		unsigned int color_texture_id;
		unsigned int depth_texture_id;
	
	public:
		fbo ();
		fbo (int, int);
		void load (int, int);
		unsigned int get_fbo_id ();
		unsigned int get_texture ();
		unsigned int get_depth_texture ();
};

#endif
