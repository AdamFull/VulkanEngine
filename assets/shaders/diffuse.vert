#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBinormal;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outLightVec;
layout(location = 2) out vec3 outLightVecB;
layout(location = 3) out vec3 outLightDir;
layout(location = 4) out vec3 outViewVec;

layout(binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  vec4 lightPosition;
  vec4 viewPosition;
} ubo;

void main() 
{
  vec3 fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
  outLightDir = normalize(ubo.lightPosition.xyz - fragPos);

  vec3 biTangent = cross(inNormal, inTangent);

  mat3 normal = transpose(inverse(mat3(ubo.model * ubo.view)));

  mat3 tbn;
	tbn[0] =  mat3(normal) * inTangent;
	tbn[1] =  mat3(normal) * biTangent;
	tbn[2] =  mat3(normal) * inNormal;

  outLightVec = vec3(ubo.lightPosition.xyz - fragPos) * tbn;

  vec3 lightDist = ubo.lightPosition.xyz - inPosition;
	outLightVecB.x = dot(inTangent, lightDist);
	outLightVecB.y = dot(biTangent, lightDist);
	outLightVecB.z = dot(inNormal, lightDist);

  outViewVec = ubo.viewPosition.xyz * tbn;

	outTexCoord = inTexCoord;
	gl_Position =  ubo.projection * ubo.model * ubo.view * vec4(inPosition, 1.0);
}