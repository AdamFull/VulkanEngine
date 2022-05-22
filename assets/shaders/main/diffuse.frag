#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout(binding = 1) uniform sampler2D color_tex;
layout(binding = 2) uniform sampler2D metalRough_tex;
layout(binding = 3) uniform sampler2D emissive_tex;
layout(binding = 4) uniform sampler2D normal_tex;
layout(binding = 5) uniform sampler2D height_tex;
layout(binding = 6) uniform sampler2D ao_tex;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec4 inTangent;

layout (location = 0) out float outMask;
layout (location = 1) out uvec4 outPack;
layout (location = 2) out vec4 outEmission;

#include "../shared_shaders.glsl"
#include "../shader_util.glsl"

void main() 
{
	outMask = 1.0;

	vec3 normal_map = getNormalsOld(normal_tex, inNormal, inTangent, inUV);
	vec3 albedo_map = texture(color_tex, inUV).rgb;

	vec4 metalRough = texture(metalRough_tex, inUV);
	float metal = metalRough.b;
	float rough = metalRough.g;
	float occlusion = texture(ao_tex, inUV).r;
	float height = texture(height_tex, inUV).r;

	vec4 pbr_map = vec4(metal, rough, occlusion, height);

	outPack = packTextures(normal_map, albedo_map, pbr_map);

	outEmission = texture(emissive_tex, inUV);
}