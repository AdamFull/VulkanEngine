#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform FUniformData 
{
  mat4 transform; // projection * view * model
  mat4 normalMatrix;
  vec3 light_direction;
} ubo;

const float ambient = 0.8;

void main() 
{
    gl_Position = ubo.transform * vec4(inPosition, 1.0);

    vec3 world_normal = normalize(mat3(ubo.normalMatrix) * inNormal);

    float light_pow = ambient + max(dot(world_normal, ubo.light_direction), 0);
    fragColor = vec4(inColor * light_pow, 1.0);

    fragTexCoord = inTexCoord;
}