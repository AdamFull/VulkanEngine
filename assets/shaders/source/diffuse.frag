#version 450
layout(set = 1, binding = 0) uniform sampler2D color_tex;
layout(set = 1, binding = 1) uniform sampler2D metalRough_tex;
layout(set = 1, binding = 2) uniform sampler2D emissive_tex;
layout(set = 1, binding = 3) uniform sampler2D normal_tex;
layout(set = 1, binding = 4) uniform sampler2D height_tex;
layout(set = 1, binding = 5) uniform sampler2D ao_tex;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inWorldPos;
layout (location = 4) in vec3 inTangent;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outMask;
layout (location = 2) out vec4 outNormal;
layout (location = 3) out vec4 outAlbedo;
layout (location = 4) out vec4 outMRAH;

void main() 
{
	outPosition = vec4(inWorldPos, 1.0);

	// Calculate normal in tangent space
	vec3 N = normalize(inNormal);
	vec3 T = normalize(inTangent);
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T, B, N);
	vec3 tnorm = TBN * normalize(texture(normal_tex, inUV).xyz * 2.0 - vec3(1.0));
	outNormal = vec4(tnorm, 1.0);

	outMask = vec4(1.0);
	outAlbedo = texture(color_tex, inUV);

	vec4 metalRough = texture(metalRough_tex, inUV);
	//float metal = metalRough.r;
	//float rough = metalRough.g;
	float occlusion = texture(ao_tex, inUV).r;
	float height = texture(height_tex, inUV).r;

	outMRAH = vec4(metalRough.gb, occlusion, 1.0);
}