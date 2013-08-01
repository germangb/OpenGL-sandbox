#version 120

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform mat4 model_view_projection_matrix;
uniform mat3 normal_matrix;

varying vec3 color;
varying vec3 vertex;
varying vec3 normal;

void main () {
	gl_Position = model_view_projection_matrix * gl_Vertex;
	color = gl_Color.rgb;
	vertex = (gl_Vertex).xyz;
	normal = gl_Normal;
}

