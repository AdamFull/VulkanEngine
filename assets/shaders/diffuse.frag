#version 450

layout(binding = 1) uniform sampler2D color_tex;
layout(binding = 2) uniform sampler2D normal_tex;
layout(binding = 3) uniform sampler2D specular_tex;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inLightVec;
layout(location = 2) in vec3 inLightVecB;
layout(location = 3) in vec3 inLightDir;
layout(location = 4) in vec3 inViewVec;

layout(location = 0) out vec4 outColor;

#define lightRadius 45.0
const float ambient = 0.25;
const float gamma = 2.2;

void main() 
{
    float invRadius = 1.0/lightRadius;
	float ambient = 0.25;

    vec3 normal = texture(normal_tex, inTexCoord).rgb * 2.0 - vec3(1.0);

	vec3 color = texture(color_tex, inTexCoord).rgb;
    
	float distSqr = dot(inLightVecB, inLightVecB);
	vec3 lVec = inLightVecB * inversesqrt(distSqr);

	float atten = max(clamp(1.0 - invRadius * sqrt(distSqr), 0.0, 1.0), ambient);
	float diffuse = clamp(dot(inLightDir, normal), 0.0, 1.0);

	vec3 light = normalize(-inLightVec);
	vec3 view = normalize(inViewVec);
	vec3 reflectDir = reflect(-light, normal);
		
    vec3 specularColor = texture(specular_tex, inTexCoord).rgb;
	float specular = pow(max(dot(view, reflectDir), 0.0), 4.0);

    outColor = vec4((color * atten + (diffuse * color + 0.5 * specular * specularColor.rgb)) * atten, 1.0);
	//outColor = vec4(pow((ambient + diffuse + specular), vec3(1.0/gamma)), 1.0);
}
