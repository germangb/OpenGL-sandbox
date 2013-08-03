#version 120

uniform sampler2D texture;
uniform vec2 resolution;

void main () {
	vec2 uv = gl_FragCoord.xy / resolution;
	vec3 color = texture2D(texture, uv).rgb;
	gl_FragColor = vec4(color, 1.0);
}
