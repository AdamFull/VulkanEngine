#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

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
layout (location = 4) in vec4 inTangent;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outMask;
layout (location = 2) out vec4 outNormal;
layout (location = 3) out vec4 outAlbedo;
layout (location = 4) out vec4 outEmission;
layout (location = 5) out vec4 outMRAH;

#include "../shared_shaders.glsl"

const float NEAR_PLANE = 0.01f;
const float FAR_PLANE = 1024.0f;

float linearDepth(float depth) 
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));
}

void main() 
{
	outPosition = vec4(inWorldPos, 1.0);

	//outNormal = vec4(normalize(inNormal), 1.0);
	outNormal = vec4(getNormalsOld(normal_tex, inNormal, inTangent, inUV), 1.0);
	//outNormal = vec4(getNormals(normal_tex, inWorldPos, inNormal, inUV), 1.0);

	outMask = vec4(1.0);
	outAlbedo = texture(color_tex, inUV);
	//outAlbedo = vec4(mix(outAlbedo.rgb, inColor, 0.5), outAlbedo.a);

	vec4 metalRough = texture(metalRough_tex, inUV);
	float metal = metalRough.b;
	float rough = metalRough.g;
	float occlusion = texture(ao_tex, inUV).r;
	float height = texture(height_tex, inUV).r;

	outEmission = texture(emissive_tex, inUV);
	outMRAH = vec4(metal, rough, occlusion, 1.0);
}