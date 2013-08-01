#include <iostream>
#include <vector>
#include <GLee.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/transform.hpp"
#ifndef PIPELINE_H
#define PIPELINE_H

enum MATRIX_MODE {
	MODEL_MATRIX = 0,
	VIEW_MATRIX,
	PROJECTION_MATRIX
};

class pipeline {
	private:
		// basic matrices
		std::vector<glm::mat4> model_matrix_stack;
		std::vector<glm::mat4> view_matrix_stack;
		std::vector<glm::mat4> projection_matrix_stack;
		
		// modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
		MATRIX_MODE current_matrix;
		glm::mat4 model_view_matrix;
		glm::mat4 model_view_projection_matrix;
		glm::mat3 normal_matrix;
	
	public:
		pipeline();	// set up the matrices
		void load_identity ();	// set the identity matrices
		void matrix_mode (int m);
		void update_glsl (unsigned int program);
		
		// transformations
		void push_matrix();
		void pop_matrix();
		void translate (float x, float y, float z);
		void rotateX (float angle);
		void rotateY (float angle);
		void rotateZ (float angle);
		
		// projection
		void perspective (float fov, float aspect, float near, float far);
		void ortho (float left, float right, float bottom, float top, float near, float far);
		
		// matrices
		glm::mat4& get_view_matrix();
		glm::mat4& get_projection_matrix();
		glm::mat4& get_model_matrix();
		
};

#endif
