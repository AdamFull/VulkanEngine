#version 450
#extension GL_EXT_scalar_block_layout : enable
#define PI 3.1415926535897932384626433832795
const float MAX_REFLECTION_LOD = 10.0; // todo: param/const

layout (set = 1, binding = 0) uniform sampler2D brdflut_tex;
layout (set = 1, binding = 1) uniform samplerCube irradiance_tex;
layout (set = 1, binding = 2) uniform samplerCube prefiltred_tex;

layout (set = 1, binding = 3) uniform sampler2D position_tex;
layout (set = 1, binding = 4) uniform sampler2D lightning_mask_tex;
layout (set = 1, binding = 5) uniform sampler2D normal_tex;
layout (set = 1, binding = 6) uniform sampler2D albedo_tex;
layout (set = 1, binding = 7) uniform sampler2D emission_tex;
layout (set = 1, binding = 8) uniform sampler2D mrah_tex;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragcolor;

struct Light {
	vec3 position;
	vec3 color;
	float radius;
};

layout(std430, set = 0, binding = 0) uniform UBO 
{
	vec3 viewPos;
	Light lights[1024];
	int lightCount;
	float gamma;
	float exposure;
} ubo;

#define ALBEDO_COLOR pow(texture(albedo_tex, inUV).rgb, vec3(2.2))

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
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
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(prefiltred_tex, R, lodf).rgb;
	vec3 b = textureLod(prefiltred_tex, R, lodc).rgb;
	return mix(a, b, lod - lodf);
}

vec3 specularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

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
		color += (kD * ALBEDO_COLOR / PI + spec) * dotNL;
	}

	return color;
}

void main() 
{
	// Get G-Buffer values
	vec3 inWorldPos = texture(position_tex, inUV).rgb;
	float mask = texture(lightning_mask_tex, inUV).r;
	vec3 N = texture(normal_tex, inUV).rgb;
	vec4 emission = texture(emission_tex, inUV);
	vec4 mrah = texture(mrah_tex, inUV);

	float metalic = mrah.r;
	float roughness = mrah.g;
	float occlusion = mrah.b;
	float height = mrah.a;
	
	//outFragcolor = vec4(N, 1.0);
	//return;

	// Calculate direction from fragment to viewPosition
    vec3 V = normalize(ubo.viewPos - inWorldPos);
	// Reflection vector
    vec3 R = reflect(-V, N);

	vec3 F0 = vec3(0.04); 
	// Reflectance at normal incidence angle
    F0 = mix(F0, ALBEDO_COLOR.rgb, metalic);

    // Light contribution
    vec3 Lo = vec3(0.0);

	for(int i = 0; i < ubo.lightCount * mask; i++) 
	{
		vec3 L = normalize(ubo.lights[i].position - inWorldPos);
		//float atten = ubo.lights[i].radius / (pow(length(L), 2.0) + 1.0);
		float atten = clamp(1.0 - pow(length(L), 2.0)/pow(ubo.lights[i].radius, 2.0), 0.0, 1.0);;
		Lo += specularContribution(L, V, N, F0, metalic, roughness) * atten * ubo.lights[i].color;
	}

	vec2 brdf = texture(brdflut_tex, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 reflection = prefilteredReflection(R, roughness).rgb;	
	vec3 irradiance = texture(irradiance_tex, N).rgb;

	// Diffuse based on irradiance
	vec3 diffuse = irradiance * ALBEDO_COLOR;	

	vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	vec3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	vec3 kD = 1.0 - F;
	kD *= 1.0 - metalic;	  
	vec3 ambient = (kD * diffuse + specular)/* * vec3(occlusion)*/;

	// Ambient part
	vec3 fragcolor = mask > 0 ? (ambient + Lo) : ALBEDO_COLOR.rgb;
	fragcolor += emission.rgb;

	// Tone mapping
	fragcolor = Uncharted2Tonemap(fragcolor * ubo.exposure);
	fragcolor = fragcolor * (1.0 / Uncharted2Tonemap(vec3(11.2)));	
	// Gamma correction
	fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
   
  	outFragcolor = vec4(fragcolor, 1.0 );	
}