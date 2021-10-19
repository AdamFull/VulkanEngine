#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBinormal;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec3 lightColor;
layout(location = 3) out vec3 fragPos;
layout(location = 4) out vec3 viewPos;
layout(location = 5) out vec3 lightPos;
layout(location = 6) out mat3 outTBN;

layout(binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  mat3 normalMatrix;
  
  vec4 viewPosition;
  vec4 lightPosition;
  vec4 lightColor;
} ubo;

void main() 
{
  fragPos = vec3(ubo.model * ubo.view * vec4(inPosition, 1.0));
  fragTexCoord = inTexCoord;
  fragColor = inColor;
  lightColor = vec3(ubo.lightColor);

  vec3 T = normalize(ubo.normalMatrix * inTangent);
  vec3 N = normalize(ubo.normalMatrix * inNormal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  outTBN = transpose(mat3(T, B, N));

  viewPos = outTBN * vec3(ubo.viewPosition);
  lightPos = outTBN * vec3(ubo.lightPosition);
  fragPos = outTBN * fragPos;
  
  gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
}