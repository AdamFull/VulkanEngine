#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

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

layout(std140, set = 0, binding = 0) uniform UBO 
{
	vec3 viewPos;
	Light lights[1024];
	int lightCount;
	float gamma;
	float exposure;
} ubo;

#include "shared_lightning.glsl"

void main() 
{
	// Get G-Buffer values
	vec3 inWorldPos = texture(position_tex, inUV).rgb;
	float mask = texture(lightning_mask_tex, inUV).r;
	vec3 albedo = pow(texture(albedo_tex, inUV).rgb, vec3(2.2));
	vec3 N = texture(normal_tex, inUV).rgb;
	vec3 emission = pow(texture(emission_tex, inUV).rgb, vec3(2.2));
	vec4 mrah = texture(mrah_tex, inUV);

	float metalic = mrah.r;
	float roughness = mrah.g;
	float occlusion = mrah.b;
	float height = mrah.a;

	bool ignoreLightning = mask == 0.0;

	vec3 fragcolor = vec3(0.0);
	if(!ignoreLightning && N != vec3(0.0f))
	{
		vec3 cameraPos = ubo.viewPos;
		N = normalize(N);
		// Calculate direction from fragment to viewPosition
		vec3 V = normalize(cameraPos - inWorldPos);
		// Reflection vector
		vec3 R = reflect(-V, N);

		//outFragcolor = vec4(R, 1.0);
		//return;

		vec3 F0 = vec3(0.04); 
		// Reflectance at normal incidence angle
		F0 = mix(F0, albedo, metalic);

		// Light contribution
		vec3 Lo = vec3(0.0);

		for(int i = 0; i < ubo.lightCount; i++) 
		{
			vec3 L = ubo.lights[i].position - inWorldPos;
			//float atten = ubo.lights[i].radius / (pow(length(L), 2.0) + 1.0);
			float atten = clamp(1.0 - pow(length(L), 2.0)/pow(ubo.lights[i].radius, 2.0), 0.0, 1.0);;
			Lo += atten * ubo.lights[i].color * specularContribution(albedo, L, V, N, F0, metalic, roughness);
		}

		vec2 brdf = texture(brdflut_tex, vec2(max(dot(N, V), 0.0), roughness)).rg;
		vec3 reflection = prefilteredReflection(R, roughness, prefiltred_tex);	
		vec3 irradiance = pow(texture(irradiance_tex, N).rgb, vec3(2.2));

		// Diffuse based on irradiance
		vec3 diffuse = irradiance * albedo;	

		vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

		// Specular reflectance
		vec3 specular = reflection * (F * brdf.r + brdf.g);

		// Ambient part
		vec3 kD = 1.0 - F;
		kD *= 1.0 - metalic;	  
		vec3 ambient = (kD * diffuse + specular)/* * vec3(occlusion)*/;
		// Ambient part
		fragcolor = (ambient + Lo);
		fragcolor += emission;
	}
	else
	{
		fragcolor = albedo;
	}

	// Tone mapping
	fragcolor = Uncharted2Tonemap(fragcolor * ubo.exposure);
	fragcolor = fragcolor * (1.0 / Uncharted2Tonemap(vec3(11.2)));	
	// Gamma correction
	fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
   
  	outFragcolor = vec4(fragcolor, 1.0 );	
}