#version 450

layout(binding = 1) uniform sampler2D color_tex;
layout(binding = 2) uniform sampler2D normal_tex;
layout(binding = 3) uniform sampler2D specular_tex;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 lightColor;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec3 viewPos;
layout(location = 5) in vec3 lightPos;
layout(location = 6) in mat3 inTBN;

layout(location = 0) out vec4 outColor;

const float ambientStrength = 0.1;
const float gamma = 2.2;

void main() 
{
    vec3 normal = (texture(normal_tex, fragTexCoord).rgb * 2.0 - 1.0);

	vec3 color = texture(color_tex, fragTexCoord).rgb;
    vec3 ambient = ambientStrength * color;
    
	vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);

    float dist = length(lightDir);
    float atten = 5.0 / (pow(dist, 2.0) + 1.0);

    vec3 diffuse = diff * color * atten;

	vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.1) * spec * texture(specular_tex, fragTexCoord).rgb * atten; 

	outColor = vec4(pow((ambient + diffuse + specular), vec3(1.0/gamma)), 1.0);
}
