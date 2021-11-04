#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec4 inJoint;
layout(location = 6) in vec4 inWeights;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec3 lightColor;
layout(location = 3) out vec3 fragPos;
layout(location = 4) out vec3 viewPos;
layout(location = 5) out vec3 lightPos;

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
  fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
  fragTexCoord = inTexCoord;
  fragColor = vec4(inColor, 1.0);

  vec3 T = normalize(mat3(ubo.normal) * inTangent);
  vec3 N = normalize(mat3(ubo.normal) * inNormal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  mat3 tbn = transpose(mat3(T, B, N));

  viewPos = tbn * ubo.viewPosition.xyz;
  lightPos = tbn * ubo.lightPosition.xyz;
  fragPos = tbn * fragPos;
  
  gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
}