#version 450
#extension GL_GOOGLE_include_directive : require

#include "diffuse_shared.glsl"

layout(push_constant) uniform UBOMaterial
{
	float alphaCutoff;
	vec3 emissiveFactor;
	float normalScale;
	float occlusionStrenth;
	vec4 baseColorFactor;
	float metallicFactor;
	float roughnessFactor;
	float tessLevel;
	float tessStrength;
} ubo;

layout(std140, binding = 0) uniform FUniformData 
{
  	mat4 model;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
} data;
 
layout (vertices = 3) out;

layout (location = 0) in vec2 inUV[];
layout (location = 1) in vec3 inColor[];
#ifdef HAS_NORMALS
layout (location = 2) in vec3 inNormal[];
#endif
#ifdef HAS_TANGENTS
layout (location = 3) in vec4 inTangent[];
#endif

layout (location = 0) out vec2 outUV[3];
layout (location = 1) out vec3 outColor[3];
#ifdef HAS_NORMALS
layout (location = 2) out vec3 outNormal[3];
#endif
#ifdef HAS_TANGENTS
layout (location = 3) out vec4 outTangent[];
#endif

void main()
{
	float dist = distance(data.viewDir, (data.model * gl_in[gl_InvocationID].gl_Position).xyz);
	float quadLevel = 1.2 * ubo.tessLevel * ubo.tessLevel;
	float tessLod = clamp(quadLevel / dist, 1.0, ubo.tessLevel);

	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = tessLod;
		gl_TessLevelOuter[0] = tessLod;
		gl_TessLevelOuter[1] = tessLod;
		gl_TessLevelOuter[2] = tessLod;		
	}

	gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
    outUV[gl_InvocationID] = inUV[gl_InvocationID];
    outColor[gl_InvocationID] = inColor[gl_InvocationID];
#ifdef HAS_NORMALS
	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
#endif
#ifdef HAS_TANGENTS
    outTangent[gl_InvocationID] = inTangent[gl_InvocationID];
#endif
} 