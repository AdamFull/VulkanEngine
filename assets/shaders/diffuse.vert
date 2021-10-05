#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out float diffuse_comp;
layout(location = 3) out float specular_comp;

layout(binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  vec4 viewPosition;
} ubo;

const vec4 lightPosition = vec4(-0.1, -3.0, -0.6, 1.0);

void main() 
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
    vec4 fragPosition = ubo.model * vec4(inPosition, 1.0);

    vec4 norm = normalize(vec4(inNormal, 1.0));
    vec4 lightDir = normalize(lightPosition - fragPosition);
    diffuse_comp = max(dot(norm, lightDir), 0.0);

    vec4 viewDir = normalize(ubo.viewPosition - fragPosition);
    vec4 reflectDir = reflect(-lightDir, norm);  
    specular_comp = pow(max(dot(viewDir, reflectDir), 0.0), 0.5);

    fragColor = inColor;
    fragTexCoord = inTexCoord;
}