#version 120

uniform sampler2D Sample0;	// View space position data
uniform sampler2D Sample1;	// View space normal vectors
uniform sampler2D Sample2;	// Normalmap to randomize the sampling kernel
uniform vec2 TexelSize;
uniform vec2 resolution;

const float OccluderBias = 0.04;
const float SamplingRadius = 5;
const vec2 Attenuation = vec2(1.);


float SamplePixels (vec3 srcPosition, vec3 srcNormal, vec2 uv)
{
	// Get the 3D position of the destination pixel
	vec3 dstPosition = texture2D(Sample0, uv).xyz;

	// Calculate ambient occlusion amount between these two points
	// It is simular to diffuse lighting. Objects directly above the fragment cast
	// the hardest shadow and objects closer to the horizon have minimal effect.
	vec3 positionVec = dstPosition - srcPosition;
	float intensity = max(dot(normalize(positionVec), srcNormal) - OccluderBias, 0.0);

	// Attenuate the occlusion, similar to how you attenuate a light source.
	// The further the distance between points, the less effect AO has on the fragment.
	float dist = length(positionVec);
	float attenuation = 1.0 / (Attenuation.x + (Attenuation.y * dist));
	
	return intensity * attenuation;
}


/// <summary>
/// Fragment shader entry.
/// <summary>
void main ()
{
	
	vec2 uv = gl_FragCoord.xy/resolution;
	// Get position and normal vector for this fragment
	vec3 srcPosition = texture2D(Sample0, uv).xyz;
	vec3 srcNormal = texture2D(Sample1, uv).xyz;
	vec2 randVec = normalize(texture2D(Sample2, uv).xy * 2.0 - 1.0);
	float srcDepth = texture2D(Sample0, uv).w;
	
	// The following variable specifies how many pixels we skip over after each
	// iteration in the ambient occlusion loop. We can't sample every pixel within
	// the sphere of influence because that's too slow. We only need to sample
	// some random pixels nearby to apprxomate the solution.
	//
	// Pixels far off in the distance will not sample as many pixels as those close up.
	float kernelRadius = SamplingRadius * (1.0 - srcDepth);
	
	// Sample neighbouring pixels
	vec2 kernel[4];
	kernel[0] = vec2(0.0, 1.0);		// top
	kernel[1] = vec2(1.0, 0.0);		// right
	kernel[2] = vec2(0.0, -1.0);	// bottom
	kernel[3] = vec2(-1.0, 0.0);	// left
	
	const float Sin45 = 0.707107;	// 45 degrees = sin(PI / 4)
	
	// Sample from 16 pixels, which should be enough to appromixate a result. You can
	// sample from more pixels, but it comes at the cost of performance.
	float occlusion = 0.0;
	for (int i = 0; i < 4; ++i)
	{
		vec2 k1 = reflect(kernel[i], randVec);
		vec2 k2 = vec2(k1.x * Sin45 - k1.y * Sin45,
					   k1.x * Sin45 + k1.y * Sin45);
		k1 *= TexelSize;
		k2 *= TexelSize;
		
		occlusion += SamplePixels(srcPosition, srcNormal, uv + k1 * kernelRadius);
		occlusion += SamplePixels(srcPosition, srcNormal, uv + k2 * kernelRadius * 0.75);
		occlusion += SamplePixels(srcPosition, srcNormal, uv + k1 * kernelRadius * 0.5);
		occlusion += SamplePixels(srcPosition, srcNormal,  + k2 * kernelRadius * 0.25);
	}
	
	// Average and clamp ambient occlusion
	occlusion /= 16.0;
	occlusion = clamp(occlusion, 0.0, 1.0);
	
	gl_FragColor = vec4(vec3(occlusion), 1.0);
}
