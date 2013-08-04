#version 120

uniform sampler2D texture;
uniform sampler2D shadow_map;

varying vec3 shadow_coord;
varying vec3 coord;
varying vec3 vertex;
varying vec3 normal;
varying vec3 real_normal;
varying vec3 light_direction;
varying vec3 color;
varying vec2 uv;

const vec2 poissonDisk[4] = vec2[](
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 )
);

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453) * 0.125 * 0.25;
}

void main () {
	
	vec2 uv_fake = vec2(0.);
	if (1.0-abs(real_normal.y) < 1e-7)
		uv_fake = coord.xz;
	else if (1.0-abs(real_normal.z) < 1e-7)
		uv_fake = vec2(coord.x, -coord.y);
	else if (1.0-abs(real_normal.x) < 1e-7)
		uv_fake = vec2(coord.z, -coord.y);
	
	vec3 final_color = color;
	final_color *= max(dot(normalize(normal), -normalize(light_direction)), 0.25);
	
	final_color += rand(floor(uv_fake/3)) * 0.5;
	//final_color += rand(floor(uv_fake)) * 0.33;
	//final_color += rand(floor(uv_fake*3)) * 0.33;
	
	
	float bias = 0.00125 * 1.5;
	float visibility = 1;

	for (int i=0; i<3; i++){
		if ( texture2D( shadow_map, shadow_coord.xy + poissonDisk[i]/500.0 ).z < shadow_coord.z-bias){
			visibility -= 0.15;
		}
	}
	

	gl_FragColor = vec4(final_color*visibility, 1.0);
}
