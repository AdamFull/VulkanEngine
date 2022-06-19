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
	vec4 frustumPlanes[6];
} data;

#ifdef HAS_HEIGHTMAP
layout(binding = 6) uniform sampler2D height_tex;
#endif

layout(triangles, equal_spacing , cw) in;

layout (location = 0) in vec2 inUV[];
layout (location = 1) in vec3 inColor[];
#ifdef HAS_NORMALS
layout (location = 2) in vec3 inNormal[];
#endif
#ifdef HAS_TANGENTS
layout (location = 3) in vec4 inTangent[];
#endif
 
layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outPosition;
#ifdef HAS_NORMALS
layout (location = 3) out vec3 outNormal;
#endif
#ifdef HAS_TANGENTS
layout (location = 4) out vec4 outTangent;
#endif

void main()
{
	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) + (gl_TessCoord.y * gl_in[1].gl_Position) + (gl_TessCoord.z * gl_in[2].gl_Position); 

	outUV = gl_TessCoord.x * inUV[0] + gl_TessCoord.y * inUV[1] + gl_TessCoord.z * inUV[2];
    outColor = gl_TessCoord.x * inColor[0] + gl_TessCoord.y * inColor[1] + gl_TessCoord.z * inColor[2];

#ifdef HAS_NORMALS
	outNormal = gl_TessCoord.x * inNormal[0] + gl_TessCoord.y * inNormal[1] + gl_TessCoord.z * inNormal[2]; 
#endif

#ifdef HAS_TANGENTS
    outTangent = gl_TessCoord.x * inTangent[0] + gl_TessCoord.y * inTangent[1] + gl_TessCoord.z * inTangent[2]; 
#endif

	float height = 0.0;
#ifdef HAS_HEIGHTMAP
    gl_Position.xyz += normalize(outNormal) * (max(texture(height_tex, outUV.st).r, 0.0) * material.tessellationStrength); //TODO: pass here 
#endif
				
	outPosition = (gl_Position).xyz;
	//outLightVec = normalize(material.lightPos.xyz - outEyesPos);	
		
	gl_Position = data.projection * data.view * data.model * gl_Position;
}