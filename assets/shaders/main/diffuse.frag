#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout(binding = 1) uniform sampler2D color_tex;
layout(binding = 2) uniform sampler2D rmah_tex;
layout(binding = 3) uniform sampler2D normal_tex;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;
layout(location = 2) in mat3 inTBN;

layout(location = 0) out uvec4 outPack;

#include "../shader_util.glsl"


void main() 
{
	vec3 normal_map = getTangentSpaceNormalMap(normal_tex, inTBN, inUV);
	vec3 albedo_map = texture(color_tex, inUV).rgb;
	vec4 pbr_map = texture(rmah_tex, inUV);

	outPack = packTextures(normal_map, albedo_map, pbr_map);
}