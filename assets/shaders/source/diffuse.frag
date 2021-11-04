#version 450

layout(set = 1, binding = 0) uniform sampler2D color_tex;
layout(set = 1, binding = 1) uniform sampler2D metalRough_tex;
layout(set = 1, binding = 2) uniform sampler2D specGloss_tex;
layout(set = 1, binding = 3) uniform sampler2D emissive_tex;
layout(set = 1, binding = 4) uniform sampler2D normal_tex;
layout(set = 1, binding = 5) uniform sampler2D ao_tex;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;
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

    float ao_factor = texture(ao_tex, fragTexCoord).r;
    vec4 emission = texture(emissive_tex, fragTexCoord);

	vec4 color = texture(color_tex, fragTexCoord);
    
	vec3 lightDir = normalize(lightPos - fragPos);
    float diffuse = max(dot(lightDir, normal), 0.15);

    float dist = length(lightDir);
    float atten = max(clamp(1.0 - invRadius * sqrt(dist), 0.0, 1.0), ambient);

	vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float specular = pow(max(dot(normal, halfwayDir), 0.0), 4.0);

	vec4 specularColor = texture(specGloss_tex, fragTexCoord);

	outColor = (color * atten + (diffuse * ao_factor * color + 0.5 * specular * specularColor + emission)) * atten;
}
