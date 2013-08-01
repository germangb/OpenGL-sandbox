#include "../headers/pipeline.h"

pipeline::pipeline () {
	model_matrix_stack.push_back(glm::mat4(1.0));
	view_matrix_stack.push_back(glm::mat4(1.0));
	projection_matrix_stack.push_back(glm::mat4(1.0));
}

void pipeline::load_identity () {
	if (current_matrix == MODEL_MATRIX) model_matrix_stack[model_matrix_stack.size()-1] = glm::mat4(1.0);
	else if (current_matrix == VIEW_MATRIX) view_matrix_stack[view_matrix_stack.size()-1] = glm::mat4(1.0);
	else if (current_matrix == PROJECTION_MATRIX) projection_matrix_stack[projection_matrix_stack.size()-1] = glm::mat4(1.0);
}

void pipeline::matrix_mode (int m) {
	if (m == MODEL_MATRIX) current_matrix = MODEL_MATRIX;
	else if (m == VIEW_MATRIX) current_matrix = VIEW_MATRIX;
	else if (m == PROJECTION_MATRIX) current_matrix = PROJECTION_MATRIX;
}

void pipeline::push_matrix () {
	glm::mat4 tmp;
	if (current_matrix == MODEL_MATRIX) {
		tmp = model_matrix_stack[model_matrix_stack.size()-1];
		model_matrix_stack.push_back(tmp);
	} else if (current_matrix == VIEW_MATRIX) {
		tmp = view_matrix_stack[view_matrix_stack.size()-1];
		view_matrix_stack.push_back(tmp);
	}
}

void pipeline::pop_matrix () {
	if (current_matrix == MODEL_MATRIX) {
		model_matrix_stack.pop_back();
	} else if (current_matrix == VIEW_MATRIX) {
		view_matrix_stack.pop_back();
	}
}

void pipeline::translate (float x, float y, float z) {
	if (current_matrix == MODEL_MATRIX)
		model_matrix_stack[model_matrix_stack.size()-1] *= glm::translate(x, y, z);
	else if (current_matrix == VIEW_MATRIX)
		view_matrix_stack[view_matrix_stack.size()-1] *= glm::translate(x, y, z);
}

void pipeline::rotateX (float angle) {
	if (current_matrix == MODEL_MATRIX)
		model_matrix_stack[model_matrix_stack.size()-1] *= glm::rotate(angle, 1.0f, 0.0f, 0.0f);
	else if (current_matrix == VIEW_MATRIX)
		view_matrix_stack[view_matrix_stack.size()-1] *= glm::rotate(angle, 1.0f, 0.0f, 0.0f);
}

void pipeline::rotateY (float angle) {
	if (current_matrix == MODEL_MATRIX)
		model_matrix_stack[model_matrix_stack.size()-1] *= glm::rotate(angle, 0.0f, 1.0f, 0.0f);
	else if (current_matrix == VIEW_MATRIX)
		view_matrix_stack[view_matrix_stack.size()-1] *= glm::rotate(angle, 0.0f, 1.0f, 0.0f);
}

void pipeline::rotateZ (float angle) {
	if (current_matrix == MODEL_MATRIX)
		model_matrix_stack[model_matrix_stack.size()-1] *= glm::rotate(angle, 0.0f, 0.0f, 1.0f);
	else if (current_matrix == VIEW_MATRIX)
		view_matrix_stack[view_matrix_stack.size()-1] *= glm::rotate(angle, 0.0f, 0.0f, 1.0f);
}

void pipeline::perspective (float fov, float aspect, float near, float far) {
	projection_matrix_stack[projection_matrix_stack.size()-1] = glm::perspective(fov, aspect, near, far);
}

void pipeline::ortho (float left, float right, float bottom, float top, float near, float far) {
	projection_matrix_stack[projection_matrix_stack.size()-1] = glm::ortho(left, right, bottom, top, near, far);
}

glm::mat4& pipeline::get_view_matrix () {
	return view_matrix_stack[view_matrix_stack.size()-1];
}

glm::mat4& pipeline::get_projection_matrix () {
	return projection_matrix_stack[projection_matrix_stack.size()-1];
}

glm::mat4& pipeline::get_model_matrix () {
	return model_matrix_stack[model_matrix_stack.size()-1];
}

void pipeline::update_glsl (unsigned int program) {
	// calculate matrices
	model_view_matrix = view_matrix_stack[view_matrix_stack.size()-1] * model_matrix_stack[model_matrix_stack.size()-1];
	model_view_projection_matrix = projection_matrix_stack[projection_matrix_stack.size()-1] * view_matrix_stack[view_matrix_stack.size()-1] * model_matrix_stack[model_matrix_stack.size()-1];
	normal_matrix = glm::mat3(model_view_matrix);
		
	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &model_matrix_stack[model_matrix_stack.size()-1][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &view_matrix_stack[view_matrix_stack.size()-1][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection_matrix"), 1, GL_FALSE, &projection_matrix_stack[projection_matrix_stack.size()-1][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "model_view_matrix"), 1, GL_FALSE, &model_view_matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "model_view_projection_matrix"), 1, GL_FALSE, &model_view_projection_matrix[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(program, "normal_matrix"), 1, GL_FALSE, &normal_matrix[0][0]);
}
