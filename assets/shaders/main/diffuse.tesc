#version 450
#extension GL_GOOGLE_include_directive : require

#include "diffuse_shared.glsl"

layout(std140, binding = 7) uniform UBOMaterial
{
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float alphaCutoff;
	float normalScale;
	float occlusionStrenth;
	float metallicFactor;
	float roughnessFactor;
	float tessellationFactor;
	float tessellationStrength;
} material;

layout(std140, binding = 0) uniform FUniformData 
{
  	mat4 model;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
	vec2 viewportDim;
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

// Calculate the tessellation factor based on screen space
// dimensions of the edge
float screenSpaceTessFactor(vec4 p0, vec4 p1)
{
	// Calculate edge mid point
	vec4 midPoint = 0.5 * (p0 + p1);
	// Sphere radius as distance between the control points
	float radius = distance(p0, p1) / 2.0;

	// View space
	vec4 v0 = data.view * data.model  * midPoint;

	// Project into clip space
	vec4 clip0 = (data.projection * (v0 - vec4(radius, vec3(0.0))));
	vec4 clip1 = (data.projection * (v0 + vec4(radius, vec3(0.0))));

	// Get normalized device coordinates
	clip0 /= clip0.w;
	clip1 /= clip1.w;

	// Convert to viewport coordinates
	clip0.xy *= data.viewportDim;
	clip1.xy *= data.viewportDim;
	
	// Return the tessellation factor based on the screen size 
	// given by the distance of the two edge control points in screen space
	// and a reference (min.) tessellation size for the edge set by the application
	return clamp(distance(clip0, clip1) / 20.0 * material.tessellationFactor, 1.0, 64.0);
}

void main()
{
	/*float dist = distance(data.viewDir, (data.model * gl_in[gl_InvocationID].gl_Position).xyz);
	float quadLevel = 1.2 * material.tessLevel * material.tessLevel;
	float tessLod = clamp(quadLevel / dist, 1.0, 64.0);

	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = tessLod;
		gl_TessLevelOuter[0] = tessLod;
		gl_TessLevelOuter[1] = tessLod;
		gl_TessLevelOuter[2] = tessLod;		
	}*/

	if (material.tessellationFactor > 0.0)
	{
		gl_TessLevelOuter[0] = screenSpaceTessFactor(gl_in[3].gl_Position, gl_in[0].gl_Position);
		gl_TessLevelOuter[1] = screenSpaceTessFactor(gl_in[0].gl_Position, gl_in[1].gl_Position);
		gl_TessLevelOuter[2] = screenSpaceTessFactor(gl_in[1].gl_Position, gl_in[2].gl_Position);
		gl_TessLevelOuter[3] = screenSpaceTessFactor(gl_in[2].gl_Position, gl_in[3].gl_Position);
		gl_TessLevelInner[0] = mix(gl_TessLevelOuter[0], gl_TessLevelOuter[3], 0.5);
		gl_TessLevelInner[1] = mix(gl_TessLevelOuter[2], gl_TessLevelOuter[1], 0.5);
	}
	else
	{
		// Tessellation factor can be set to zero by example
		// to demonstrate a simple passthrough
		gl_TessLevelInner[0] = 1.0;
		gl_TessLevelInner[1] = 1.0;
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = 1.0;
		gl_TessLevelOuter[2] = 1.0;
		gl_TessLevelOuter[3] = 1.0;
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