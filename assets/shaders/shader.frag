#version 450

layout(binding = 1) uniform sampler2D color;
layout(binding = 2) uniform sampler2D normal;
layout(binding = 3) uniform sampler2D specular;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec4 diffuse = fragColor * texture(color, fragTexCoord);
    vec4 specular = vec4(0.8) * texture(specular, fragTexCoord);
    outColor = vec4(diffuse + specular);
}
