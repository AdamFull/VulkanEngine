#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNormals;
layout(location = 3) out vec3 objColor;
layout(location = 4) out vec3 lightPos;

layout(binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  vec4 lightPosition;
} ubo;

void main() 
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    fragNormals = inNormal;
    objColor = inColor;
    lightPos = vec3(ubo.lightPosition);

    fragTexCoord = inTexCoord;
}