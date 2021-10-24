#version 450

layout(set = 2, binding = 0) uniform sampler2D color_tex;
layout(set = 2, binding = 1) uniform sampler2D normal_tex;
layout(set = 2, binding = 2) uniform sampler2D specular_tex;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 lightColor;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec3 viewPos;
layout(location = 5) in vec3 lightPos;

layout(location = 0) out vec4 outColor;

#define lightRadius 30.0
const float ambient = 0.1;
const float gamma = 2.2;

void main() 
{
	float invRadius = 1.0/lightRadius;
    vec3 normal = (texture(normal_tex, fragTexCoord).rgb * 2.0 - 1.0);

	vec3 color = texture(color_tex, fragTexCoord).rgb;
    
	vec3 lightDir = normalize(lightPos - fragPos);
    float diffuse = max(dot(lightDir, normal), 0.0);

    float dist = length(lightDir);
    float atten = max(clamp(1.0 - invRadius * sqrt(dist), 0.0, 1.0), ambient);

	vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float specular = pow(max(dot(normal, halfwayDir), 0.0), 4.0);

	vec3 specularColor = texture(specular_tex, fragTexCoord).rgb;

	outColor = vec4((color * atten + (diffuse * color + 0.5 * specular * specularColor)) * atten, 1.0);
}
