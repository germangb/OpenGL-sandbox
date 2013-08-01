#include "../headers/camera.h"

camera::camera (pipeline* _p) {
	p = _p;
	velocity = 0.5;
	animate = true;
	
	x = dx = 0;
	y = dy = 0;
	z = dz = 0;
	
	rx = drx = 0;
	ry = dry = 0;
	rz = drz = 0;
}

void camera::set_fov (float f) {
	fov = f;
}

void camera::set_near (float f) {
	near = f;
}

void camera::set_far (float f) {
	far = f;
}

void camera::set_aspect_ratio (float f) {
	aspect_ratio = f;
}

void camera::use () {
	
	p->matrix_mode(PROJECTION_MATRIX);
	p->load_identity();
	p->perspective(fov, aspect_ratio, near, far);
	
	p->matrix_mode(VIEW_MATRIX);
	p->load_identity();
	p->rotateX(rx);
	p->rotateY(ry);
	p->rotateZ(rz);
	p->translate(-x, -y, -z);
	
	glViewport(0, 0, viewport_x, viewport_y);
}

void camera::set_viewport (float vp_x, float vp_y) {
	viewport_x = vp_x;
	viewport_y = vp_y;
}

void camera::update () {
	
	if (dry > 360) {
		dry -= 360;
		ry -= 360;
	}
	
	if (dry < -360) {
		dry += 360;
		ry += 360;
	}

	float mx = (dx - x)/6.;
	float my = (dy - y)/6.;
	float mz = (dz - z)/6.;
	
	if (std::abs(mx) > 1.) {
		if (mx < 0) mx = -1.;
		else mx = 1.;
	}
	
	if (std::abs(my) > 1.) {
		if (my < 0) my = -1.;
		else my = 1.;
	}
	
	if (std::abs(mz) > 1.) {
		if (mz < 0) mz = -1.;
		else mz = 1.;
	}
	
	x += mx;
	y += my;
	z += mz;
	
	if (std::abs(dx-x) < 1e-3) x = dx;
	if (std::abs(dy-y) < 1e-3) y = dy;
	if (std::abs(dz-z) < 1e-3) z = dz;
	
	float mrx = (drx - rx)/6.;
	float mry = (dry - ry)/6.;
	float mrz = (drz - rz)/6.;
	
	if (std::abs(mrx) > 1.) {
		if (mrx < 0) mrx = -1.;
		else mrx = 1.;
	}
	
	if (std::abs(mry) > 1.) {
		if (mry < 0) mry = -1.;
		else mry = 1.;
	}
	
	if (std::abs(mrz) > 1.) {
		if (mrz < 0) mrz = -1;
		else mrz = 1.;
	}
	
	rx += mrx;
	ry += mry;
	rz += mrz;
	
	if (std::abs(drx-rx) < 1e-3) rx = drx;
	if (std::abs(dry-ry) < 1e-3) ry = dry;
	if (std::abs(drz-rz) < 1e-3) rz = drz;
}

void camera::set_animate (bool b) {
	animate = b;
}

void camera::set_position (const float nx, const float ny, const float nz) {
	dx = x = nx;
	dy = y = ny;
	dz = z = nz;
}

void camera::set_rotation (const float nrx, const float nry, const float nrz) {
	drx = rx = nrx;
	dry = ry = nry;
	drz = rz = nrz;
}

void camera::d_position (float _dx, float _dy, float _dz) {
	dx += _dx;
	dy += _dy;
	dz += _dz;
}

void camera::d_rotation (const float ddrx, const float ddry, const float ddrz) {
	drx += ddrx;
	dry += ddry;
	drz += ddrz;
}

void camera::set_x (const float _x) {
	dx = _x;
	if (not animate) x = dx;
}

void camera::set_y (const float _y) {
	dy = _y;
	if (not animate) y = dy;
}

void camera::set_z (const float _z) {
	dz = _z;
	if (not animate) z = dz;
}

void camera::set_rx (const float _rx) {
	drx = _rx;
	if (not animate) rx = drx;
}

void camera::set_ry (const float _ry) {
	dry = _ry;
	if (not animate) ry = dry;
}

void camera::set_rz (const float _rz) {
	drz = _rz;
	if (not animate) rz = drz;
}

void camera::move_forward(const float v) {
	dx += -sin(ry * M_PI / 180) * v;
	dz += cos(ry * M_PI / 180) * v;
}

void camera::move_backwards(const float v) {
	dx += sin(ry * M_PI / 180) * v;
	dz += -cos(ry * M_PI / 180) * v;
}

void camera::move_left (const float v) {
	dx += -sin( (ry - 90) * M_PI / 180) * v;
	dz += cos( (ry - 90) * M_PI / 180) * v;
}

void camera::move_right (const float v) {
	dx += -sin( (ry + 90) * M_PI / 180) * v;
	dz += cos( (ry + 90) * M_PI / 180) * v;
}

void camera::move_upwards (const float v) {
	dy += v;
}

void camera::move_downwards (const float v) {
	dy += -v;
}

float camera::get_x () {
	return x;
}

float camera::get_y () {
	return y;
}

float camera::get_z () {
	return z;
}

float camera::get_rx () {
	return rx;
}

float camera::get_ry () {
	return ry;
}

float camera::get_rz () {
	return rz;
}
