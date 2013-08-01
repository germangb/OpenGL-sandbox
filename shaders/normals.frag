#version 120

varying vec3 normal;

void main () {
	vec3 color = (normalize(normal)+vec3(1.))/2.;
	gl_FragColor = vec4(color, 1.0);
}
