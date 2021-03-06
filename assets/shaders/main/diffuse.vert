#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outColor;
layout (location = 3) out vec3 outWorldPos;
layout (location = 4) out vec4 outTangent;

layout(std140, binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  mat4 normal;
  vec4 instancePos[256];
  //float repeat;
} ubo;

void main() 
{
  vec4 tmpPos = vec4(inPosition, 1.0) + ubo.instancePos[gl_InstanceIndex];

  outWorldPos = (ubo.model * tmpPos).xyz;

  mat3 normal = mat3(ubo.normal);
	outUV = inTexCoord * 1.0;
  outNormal = normal * inNormal;
	outTangent = vec4(normal * inTangent.xyz, inTangent.w);

  outColor = inColor;
  
  gl_Position = ubo.projection * ubo.view * ubo.model * tmpPos;
}