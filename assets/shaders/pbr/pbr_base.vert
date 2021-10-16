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
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outTangent;
layout(location = 4) out vec3 fragPos;
layout(location = 5) out vec3 viewPos;

layout(binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  vec4 viewPosition;
} ubo;

vec3 lightPosition = vec3(0.0, -1.7, -1.137);

void main() 
{
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    viewPos = vec3(ubo.viewPosition);
    fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    gl_Position = ubo.projection * ubo.view * vec4(fragPos, 1.0);
    
    outNormal = mat3(ubo.model) * inNormal;
    outTangent = vec4(mat3(ubo.model) * inTangent.xyz, 0.0);
}