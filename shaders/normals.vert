#version 120

uniform mat4 model_view_projection_matrix;
uniform mat3 normal_matrix;

varying vec3 normal;

void main () {
	gl_Position = model_view_projection_matrix * gl_Vertex;
	normal = normal_matrix * gl_Normal;
}
