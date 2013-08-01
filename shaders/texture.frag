#version 120

uniform sampler2D texture;
uniform sampler2D texture2;
uniform vec2 resolution;
uniform float time;

const vec3 night = vec3(1,.25,0.125*0.125)*0.5;
const vec3 day = vec3(.25,.75,1)*0.25*0.25;

float line (float b, float slope, float var) {
	return (var-b)*slope;
}

vec3 day_cont (float t) {
	return mix(day, night, 1);
}

void main () {
	vec2 uv = gl_FragCoord.xy / resolution;
	vec3 final_color = texture2D(texture, uv).rgb;
	vec3 result = clamp(final_color.rgb - day_cont(time)*vec3(1,1,.9), 0.0, 1.0);
	//if (gl_FragCoord.x <= resolution.x/4. && gl_FragCoord.y <= resolution.y/4.) result =  mix(result, texture2D(texture2, uv*4.).rgb, 1);
		
	gl_FragColor = vec4(result, 1.0);
}
