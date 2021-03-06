// source: http://www.minecraftforum.net/topic/1720766-glsl-shaderpack-naelegos-cel-shaders/

#version 120

#define CEL_SHADING
#define SAMPLE_WIDTH 0.004
#define DEPTH_THRESHOLD .7

uniform sampler2D composite;
uniform sampler2D gdepth;
uniform sampler2D water_buffer;
uniform sampler2D blurred;
uniform float time;

const float near = 1.;
const float far = 128.;
const float pp_cont = 1.;

uniform vec2 resolution;
varying vec4 texcoord;

float oLinearDepth(float depth) {
        return (2.0 * near * far / ( far + near - ( 2.0 * depth - 1.0 ) * ( far - near )));
}

float oEdgeDetect(vec2 coord, vec3 col){
	
	float depth = oLinearDepth(texture2D(gdepth, coord).r);
	float nDepth = depth / far;
	float st = max((1.0 - nDepth) * SAMPLE_WIDTH, .00075);
	vec2 sample1 = coord + vec2(st, 0.0);
	vec2 sample2 = coord + vec2(0.0, st);
	vec2 sample3 = coord - vec2(st, 0.0);
	vec2 sample4 = coord - vec2(0.0, st);
	
	if(  abs(depth - ( 	

				(oLinearDepth(texture2D(gdepth, sample1).r) + 
				oLinearDepth(texture2D(gdepth, sample2).r) + 
				oLinearDepth(texture2D(gdepth, sample3).r) + 
				oLinearDepth(texture2D(gdepth, sample4).r)) / 4.0 )   
	
	) > DEPTH_THRESHOLD    ) {
		return nDepth; 
	}

	return 1.0;
}

void main()
{
	vec2 uv = gl_FragCoord.xy / resolution;
	vec2 uv_original = gl_FragCoord.xy / resolution;
	vec2 uv_offset = vec2(sin(gl_FragCoord.y*0.125+time*2)*0.00125, cos(gl_FragCoord.x*0.125+time*2)*0.0025); 
	float water = texture2D(water_buffer, uv).r;
	
	if (water < 1.) {
		uv += uv_offset;
		if ((1.0 - texture2D(water_buffer, uv).r) >= 1.) {
			uv -= uv_offset;
		}
	}
	
	vec3 color = texture2D(composite, uv).rgb;
	float edge = oEdgeDetect(uv_original, color.rgb);
	
	vec3 final_color = color*water + (1.-water) * vec3(33./255., 66./255., 70./255.);
	
	gl_FragColor = vec4(final_color*edge*edge,1);
}


/*#version 120

uniform sampler2D texture;
uniform sampler2D blurred;
uniform vec2 resolution;
uniform float time;

const vec3 night = vec3(1,.25,0.125*0.125)*0.5;
const vec3 day = vec3(.25,.75,1)*0.25*0.25*0.25;
float pp_cont = 1;

float line (float b, float slope, float var) {
	return (var-b)*slope;
}

vec3 day_cont (float t) {
	return mix(day, night, 0);
}

void main () {
	vec2 uv = gl_FragCoord.xy / resolution;
	vec3 final_color = texture2D(texture, uv).rgb * pp_cont + texture2D(blurred, uv).rgb * (1.-pp_cont);
	
	vec3 result = clamp(final_color.rgb - day_cont(time)*vec3(1,1,.9), 0.0, 1.0);

	gl_FragColor = vec4(result, 1.0);
}
*/
