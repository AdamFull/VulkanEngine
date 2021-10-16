#version 450

layout(binding = 1) uniform sampler2D color_tex;
layout(binding = 2) uniform sampler2D normal_tex;
layout(binding = 3) uniform sampler2D specular_tex;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;
layout(location = 4) in vec3 fragPos;
layout(location = 5) in vec3 viewPos;
layout(location = 6) in vec3 lightPos;

layout(location = 0) out vec4 outColor;

vec3 calculateNormal()
{
	vec3 tangentNormal = texture(normal_tex, fragTexCoord).xyz * 2.0 - 1.0;

	vec3 N = normalize(inNormal);
	vec3 T = normalize(inTangent.xyz);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNormal);
}

const float ambientStrength = 0.5;

void main() 
{
    vec3 normal = calculateNormal();

	vec3 color = texture(color_tex, fragTexCoord).rgb;
    vec3 ambient = ambientStrength * color;
    
	vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

	vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec * texture(specular_tex, fragTexCoord).rgb;

	outColor = vec4(color + ambient + specular, 1.0);
}
