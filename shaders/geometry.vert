#version 120

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform mat4 model_view_projection_matrix;
uniform mat3 normal_matrix;

uniform mat4 shadow_view;
uniform mat4 shadow_projection;
uniform mat4 shadow_fix;

varying vec3 shadow_coord;
varying vec3 coord;
varying vec3 vertex;
varying vec3 normal;
varying vec3 real_normal;
varying vec3 light_direction;
varying vec3 color;
varying vec2 uv;

void main () {
	gl_Position = model_view_projection_matrix * gl_Vertex;
	shadow_coord = (shadow_fix * shadow_projection * shadow_view * model_matrix * gl_Vertex).xyz;
	
	light_direction = (view_matrix * vec4(0, 0, -5, 0)).xyz;
	vertex = (model_view_matrix * gl_Vertex).xyz;
	normal = (model_view_matrix * vec4(gl_Normal, 0.0)).xyz;
	
	color = gl_Color.rgb;
	real_normal = gl_Normal;
	coord = (model_matrix * gl_Vertex).xyz;
	uv = gl_MultiTexCoord0.xy;
	
	light_direction = (view_matrix * vec4(-1.75, -2, -1, 0)).xyz;
}
