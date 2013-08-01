#version 120

varying vec3 color;
varying vec3 vertex;
varying vec3 normal;

uniform sampler2D texture;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453) * 0.125 * 0.25;
}

void main () {
	vec3 final_color = color;
	
	vec2 uv;
	if (1.0-abs(normal.y) < 1e-7) {
		uv = vertex.xz;
	} else if (1.0-abs(normal.z) < 1e-7) {
		uv = vec2(vertex.x, -vertex.y);
	} else if (1.0-abs(normal.x) < 1e-7) {
		uv = vec2(vertex.z, -vertex.y);
	}
	
	if (uv != vertex.xz) {
		final_color *= 1.0 - max(-(0.65*vertex.y-1), 0.0)*0.4;
	} else {
		final_color += texture2D(texture, vertex.xz/25.).rgb;
		final_color /= 2.;
	}
	
	final_color += rand(floor(uv/8.)) * 0.25;
	final_color += rand(floor(uv/2.)) * 0.25;
	final_color += rand(floor(uv)) * 0.25;
	
	gl_FragColor = vec4(final_color, 1.0);
}
