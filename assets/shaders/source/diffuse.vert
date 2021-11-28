#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;
layout(location = 5) in vec4 inJoint;
layout(location = 6) in vec4 inWeights;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outColor;
layout (location = 3) out vec3 outWorldPos;
layout (location = 4) out vec4 outTangent;

layout(set = 0, binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  mat4 normal;
} ubo;

void main() 
{
  outWorldPos = vec3(ubo.model * vec4(inPosition, 1.0));
	outUV = inTexCoord;
  outNormal = mat3(ubo.normal) * inNormal;
	outTangent = vec4(mat3(ubo.normal) * inTangent.xyz, inTangent.w);

  outColor = inColor;
  
  gl_Position = ubo.projection * ubo.view * vec4(outWorldPos, 1.0);
}