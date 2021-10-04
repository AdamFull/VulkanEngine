#version 450

layout(binding = 1) uniform sampler2D color;
layout(binding = 2) uniform sampler2D normal;
layout(binding = 3) uniform sampler2D specular;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 fragNormals;
layout(location = 3) in vec3 objColor;
layout(location = 4) in vec3 lightPos;

layout(location = 0) out vec4 outColor;

void main() 
{
    float ambientStrength = 0.5;
    vec4 incolor = texture(color, fragTexCoord);
    vec4 ambient = ambientStrength * incolor;
    vec3 lpos = vec3(-0.1, -3.0, -0.6);

    vec3 norm = normalize(fragNormals);
    vec3 lightDir = normalize(lpos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * incolor;

    vec3 viewDir = normalize(lightPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.5);
    vec4 specular = vec4(1.0) * spec * texture(specular, fragTexCoord);

    outColor = vec4(ambient + diffuse + specular);
}
