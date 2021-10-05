#version 450

layout(binding = 1) uniform sampler2D color_tex;
layout(binding = 2) uniform sampler2D normal_tex;
layout(binding = 3) uniform sampler2D specular_tex;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in float diffuse_comp;
layout(location = 3) in float specular_comp;

layout(location = 0) out vec4 outColor;

const float ambientStrength = 0.5;
void main() 
{
    vec4 color = texture(color_tex, fragTexCoord);
    vec4 ambient = ambientStrength * color;

    vec4 diffuse = diffuse_comp * color;

    vec4 specular = vec4(1.0) * specular_comp * texture(specular_tex, fragTexCoord);

    outColor = vec4(ambient + diffuse + specular);
}
