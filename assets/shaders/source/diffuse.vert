#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec4 inJoint;
layout(location = 6) in vec4 inWeights;

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outTangent;
layout (location = 4) out vec3 outViewPos;
layout (location = 5) out vec3 outLightPos;

layout(set = 0, binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  mat4 normal;
  
  vec4 viewPosition;
  vec4 lightPosition;
} ubo;

void main() 
{
  outWorldPos = vec3(ubo.model * vec4(inPosition, 1.0));
  outNormal = mat3(ubo.model) * inNormal;
	outTangent = vec3(mat3(ubo.model) * inTangent);
	outUV = inTexCoord;
  outViewPos = ubo.viewPosition.xyz;
  outLightPos = ubo.lightPosition.xyz;
  
  gl_Position = ubo.projection * ubo.view * vec4(outWorldPos, 1.0);
}