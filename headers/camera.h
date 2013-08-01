#include "pipeline.h"
#include <GL/gl.h>
#include "pipeline.h"
#include <GL/glu.h>
#include <cmath>
#include <iostream>
#ifndef CAMERA_H
#define CAMERA_H

#ifndef M_PI
#define M_PI = 3.14159265;
#endif

class camera {
	private:
		float x, y, z;
		float dx, dy, dz;
		float rx, ry, rz;
		float drx, dry, drz;
		float velocity;
		bool animate;
		pipeline* p;
		
		// perspective params
		float fov;
		float aspect_ratio;
		float near;
		float far;
		
		float viewport_x;
		float viewport_y;
		
	public:
		camera (pipeline*);
		void use();	// to handle the projection and the view matrices
		void update();
		void set_animate(bool);
		void set_viewport(float, float);
		void set_fov(float);
		void set_near(float);
		void set_far(float);
		void set_aspect_ratio(float);
		void set_position (const float, const float, const float);
		void set_rotation (const float, const float, const float);
		void d_position (const float, const float, const float);
		void d_rotation (const float, const float, const float);
		void set_x (const float);
		void set_y (const float);
		void set_z (const float);
		void set_rx (const float);
		void set_ry (const float);
		void set_rz (const float);
		void move_forward (const float);
		void move_backwards(const float);
		void move_left(const float);
		void move_right(const float);
		void move_upwards(const float);
		void move_downwards(const float);
		float get_x();
		float get_y();
		float get_z();
		float get_rx();
		float get_ry();
		float get_rz();
};

#endif
