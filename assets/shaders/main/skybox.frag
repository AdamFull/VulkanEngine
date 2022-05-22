#version 450
#extension GL_GOOGLE_include_directive : require
layout (binding = 1) uniform samplerCube samplerCubeMap;

layout (location = 0) in vec3 inWorldPos;

layout (location = 0) out uvec4 outPack;
layout (location = 1) out vec4 outEmission;

#include "../shader_util.glsl"

void main() 
{
	vec3 normal_map = vec3(0.0);
	vec3 albedo_map = texture(samplerCubeMap, inWorldPos).rgb;
	outPack = packTextures(normal_map, albedo_map, vec4(0.0));

	outEmission = vec4(0.0);
}