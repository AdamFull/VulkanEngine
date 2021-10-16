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

const float ambientStrength = 0.1;
const float gamma = 2.2;

vec3 calculateNormal(mat3 tbn)
{
	vec3 tangentNormal = texture(normal_tex, fragTexCoord).xyz * 2.0 - 1.0;
	return normalize(tbn * tangentNormal);
}

mat3 calculateTBN()
{
	vec3 N = normalize(inNormal);
	vec3 T = normalize(inTangent.xyz);
	vec3 B = normalize(cross(N, T));
	return mat3(T, B, N);
}

void main() 
{
	mat3 tbn = calculateTBN();
    vec3 normal = calculateNormal(tbn);

	vec3 lightPosTangent = lightPos * normal;
	vec3 fragPosTangent = fragPos * normal;
	vec3 viewPosTangent = viewPos * normal;

	vec3 color = texture(color_tex, fragTexCoord).rgb;
    vec3 ambient = ambientStrength * color;
    
	vec3 lightDir = normalize(lightPosTangent - fragPosTangent);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

	vec3 viewDir = normalize(viewPosTangent - fragPosTangent);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec * texture(specular_tex, fragTexCoord).rgb;

	outColor = vec4(pow(color + ambient + specular, vec3(1.0/gamma)), 1.0);
}
