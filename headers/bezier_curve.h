#include <iostream>
#ifndef BEZIER_CURVE_H
#define BEZIER_CURVE_H

class curve {
	private:
		class segment;
		typedef std::vector<std::vector<segment> > VVS;
		
		VVS tree;
		segment* top_segment;		
		inline Vector2f get_pos (float, segment*);
	
	public:
		curve (int, Vector2f*);
		Vector2f get_coord (float);
		VVS* get_segments();
};

class curve::segment {
	private:
		segment* A;
		segment* B;
		bool base;
	
	public:
		Vector2f C;
		Vector2f D;
		
		segment (Vector2f, Vector2f);
		segment (segment*, segment*);
		segment ();
		bool is_base ();
		Vector2f* get_C ();
		Vector2f* get_D ();
	
	friend class curve;
};

curve::segment::segment () {
	
}

curve::segment::segment (Vector2f _C, Vector2f _D) {
	base = true;
	C = _C;
	D = _D;
}

curve::segment::segment (segment* _A, segment* _B) {
	base = false;
	A = _A;
	B = _B;
}

bool curve::segment::is_base () {
	return base;
}

curve::curve (int N, Vector2f* p) {
	tree.resize(N, std::vector<segment> (N));
	// base segments
	segment* top;
	for (int i = 0; i < N-1; ++i) {
		segment tmp(p[i], p[i+1]);
		tree[0][i] = tmp;
		top = &tree[0][i];
		//std::cerr << p[i].x << " " << p[i].y << " - " << p[i+1].x << " " << p[i+1].y << std::endl;
	}
	//std::cerr << std::endl;
	
	for (int i = 1; i < N-1; ++i) {
		for (int x = 0; x < N-i-1; ++x) {
			segment tmp(&tree[i-1][x], &tree[i-1][x+1]);
			tree[i][x] = tmp;
			top = &tree[i][x];
		}
	}
	
	top_segment = top;
}

inline Vector2f curve::get_pos (float t, segment* S) {
	Vector2f pos;
	if (S->is_base()) {
		pos.x = (1.0-t) * (S->C).x + t * (S->D).x;
		pos.y = (1.0-t) * (S->C).y + t * (S->D).y;
		return pos;
	}
	
	pos.x = (1.0-t) * get_pos(t, S->A).x + t * get_pos(t, S->B).x;
	pos.y = (1.0-t) * get_pos(t, S->A).y + t * get_pos(t, S->B).y;
	return pos;
}

Vector2f curve::get_coord (float t) {
	return get_pos(t, top_segment);
}

curve::VVS* curve::get_segments () {
	return &tree;
}

#endif
