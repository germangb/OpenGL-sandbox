#version 120

uniform vec3 color;

varying float depth;

void main () {
	
	vec3 final_color = vec3(1.);
	
	if (depth < 4.) final_color *= depth * (.75 - .45) / 4. + .45;
	else if (depth >= 4. && depth <= 5) final_color *= .75;
	else final_color *= 1.0;
	
	gl_FragColor = vec4(final_color, 1.0);
}
