#version 450

//Default
layout(set = 1, binding = 0) uniform sampler2D brdflut_tex;
layout(set = 1, binding = 1) uniform samplerCube irradiate_cube_tex;
layout(set = 1, binding = 2) uniform samplerCube prefiltred_cube_tex;

//Additional
layout(set = 1, binding = 3) uniform sampler2D color_tex;
layout(set = 1, binding = 4) uniform sampler2D metalRough_tex;
layout(set = 1, binding = 5) uniform sampler2D specGloss_tex;
layout(set = 1, binding = 6) uniform sampler2D emissive_tex;
layout(set = 1, binding = 7) uniform sampler2D normal_tex;
layout(set = 1, binding = 8) uniform sampler2D height_tex;
layout(set = 1, binding = 9) uniform sampler2D ao_tex;

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
const float exposure = 1.2;

#define PI 3.1415926535897932384626433832795
#define ALBEDO pow(texture(color_tex, fragTexCoord).rgb, vec3(gamma))

// From http://filmicgames.com/archives/75
vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom); 
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
vec3 F_Schlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 prefilteredReflection(vec3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(prefiltred_cube_tex, R, lodf).rgb;
	vec3 b = textureLod(prefiltred_cube_tex, R, lodc).rgb;
	return mix(a, b, lod - lodf);
}

vec3 specularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0) {
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, F0);		
		vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);		
		vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);			
		color += (kD * ALBEDO / PI + spec) * dotNL;
	}

	return color;
}

//From https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float height_scale, float minLayers, float maxLayers)
{ 
    vec2  currentTexCoords = texCoords;
    float currentDepthMapValue = texture(height_tex, fragTexCoord).r;

    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;

    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(height_tex, fragTexCoord).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(height_tex, fragTexCoord).r - currentLayerDepth + layerDepth;
    
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;    
}

void main() 
{
	float invRadius = 1.0/lightRadius;
    vec3 normal = (texture(normal_tex, fragTexCoord).rgb * 2.0 - 1.0);

    vec3 emission = texture(emissive_tex, fragTexCoord).rgb;

    float metallic = texture(metalRough_tex, fragTexCoord).r;
	float roughness = texture(metalRough_tex, fragTexCoord).g;

    vec3 F0 = vec3(0.04); 
	F0 = mix(F0, ALBEDO, metallic);

    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
	vec3 Lo = specularContribution(lightDir, viewDir, normal, F0, metallic, roughness);

    vec2 brdf = texture(brdflut_tex, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(reflectDir, roughness);	
	vec3 irradiance = texture(irradiate_cube_tex, normal).rgb;

    vec3 diffuse = irradiance * ALBEDO;

    vec3 F = F_SchlickR(max(dot(normal, viewDir), 0.0), F0, roughness);

    // Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

    float dist = length(lightDir);
    float atten = max(clamp(1.0 - invRadius * sqrt(dist), 0.0, 1.0), ambient);

    // Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metallic;	  
	vec3 ambient = (kD * diffuse + specular) * texture(ao_tex, fragTexCoord).rrr;
    
    //vec3 halfwayDir = normalize(lightDir + viewDir);  
    //float specular = pow(max(dot(normal, halfwayDir), 0.0), 4.0);

    vec3 color = ambient + Lo;

    // Tone mapping
	color = Uncharted2Tonemap(color * exposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / gamma));

	//vec4 specularColor = texture(specGloss_tex, fragTexCoord);

	outColor = vec4(color, 1.0);
}