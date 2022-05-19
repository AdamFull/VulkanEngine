#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

layout (binding = 0) uniform sampler2D brdflut_tex;
layout (binding = 1) uniform samplerCube irradiance_tex;
layout (binding = 2) uniform samplerCube prefiltred_tex;

layout (input_attachment_index = 0, binding = 3) uniform subpassInput position_tex;
layout (input_attachment_index = 1, binding = 4) uniform subpassInput lightning_mask_tex;
layout (input_attachment_index = 2, binding = 5) uniform subpassInput normal_tex;
layout (input_attachment_index = 3, binding = 6) uniform subpassInput albedo_tex;
layout (input_attachment_index = 4, binding = 7) uniform subpassInput emission_tex;
layout (input_attachment_index = 5, binding = 8) uniform subpassInput mrah_tex;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragcolor;
layout (location = 1) out vec4 outBrightness;

struct PointLight
{
	vec3 position;
	vec3 color;
	float radius;
	float intencity;
};

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float intencity;
};

struct SpotLight
{
	vec3 position;
	vec3 color;
	vec3 direction;
	float intencity;
	float cutoff;
};

layout(std140, binding = 9) uniform UBOLightning
{
	mat4 invViewProjection;
	vec4 viewPos;
	float bloom_threshold;
	int pointLightCount;
	int directionalLightCount;
	int spotLightCount;
} ubo;

//Lights
layout(std430, binding = 10) uniform UBOLights
{
	PointLight pointLights[32];
	DirectionalLight directionalLights[32];
	SpotLight spotLights[32];
} lights;

#include "../shared_lightning.glsl"

void main() 
{
	// Get G-Buffer values
	vec3 inWorldPos = subpassLoad(position_tex).rgb;
	float mask = subpassLoad(lightning_mask_tex).r;
	vec3 albedo = pow(subpassLoad(albedo_tex).rgb, vec3(2.2f));
	vec3 N = subpassLoad(normal_tex).rgb;
	vec3 emission = pow(subpassLoad(emission_tex).rgb, vec3(2.2f));
	vec4 mrah = subpassLoad(mrah_tex);

	float metalic = mrah.r;
	float roughness = mrah.g;
	float occlusion = mrah.b;
	float height = mrah.a;

	bool ignoreLightning = mask == 0.0f;

	vec3 fragcolor = vec3(0.0f);
	if(!ignoreLightning && N != vec3(0.0f))
	{
		vec3 cameraPos = ubo.viewPos.xyz;
		// Calculate direction from fragment to viewPosition
		vec3 V = normalize(cameraPos - inWorldPos);
		// Reflection vector
		vec3 R = reflect(-V, N);

		vec3 F0 = vec3(0.04f); 
		// Reflectance at normal incidence angle
		F0 = mix(F0, albedo, metalic);

		// Light contribution
		vec3 Lo = vec3(0.0f);

		//Adding point lights
		for(int i = 0; i < ubo.pointLightCount; i++) 
		{
			PointLight light = lights.pointLights[i];
			vec3 L = light.position - inWorldPos;
			float dist = length(L);
			L = normalize(L);
			float atten = clamp(1.0 - pow(dist, 2.0f)/pow(light.radius, 2.0f), 0.0f, 1.0f); atten *= atten;
			Lo += atten * light.color.rgb * light.intencity * specularContribution(albedo, L, V, N, F0, metalic, roughness);
		}

		//Adding directional lights
		for(int i = 0; i < ubo.directionalLightCount; i++) 
		{
			DirectionalLight light = lights.directionalLights[i];
			vec3 L = -light.direction;
			float dist = length(L);
			L = normalize(L);
			Lo += light.color.rgb * light.intencity * specularContribution(albedo, L, V, N, F0, metalic, roughness);
		}

		//Adding spot lights
		for(int i = 0; i < ubo.spotLightCount; i++) 
		{
			SpotLight light = lights.spotLights[i];
			vec3 L = normalize(inWorldPos - light.position);
			float theta = dot(L, light.direction); 
			if(theta > light.cutoff)
			{
				float dist = length(L);
				L = normalize(L);
				float atten = 1.0 - (1.0 - theta) * 1.0/(1.0 - light.cutoff);
				Lo += light.color.rgb * atten * light.intencity * specularContribution(albedo, L, V, N, F0, metalic, roughness);
			}
		}

		vec2 brdf = texture(brdflut_tex, vec2(max(dot(N, V), 0.0f), roughness)).rg;
		vec3 reflection = prefilteredReflection(R, roughness, prefiltred_tex);	
		vec3 irradiance = pow(texture(irradiance_tex, N).rgb, vec3(2.2f));

		// Diffuse based on irradiance
		vec3 diffuse = irradiance * albedo;	

		vec3 F = F_SchlickR(max(dot(N, V), 0.0f), F0, roughness);

		// Specular reflectance
		vec3 specular = reflection * (F * brdf.r + brdf.g);

		//outFragcolor = vec4(V, 1.0f );
		//return;

		// Ambient part
		vec3 kD = 1.0f - F;
		kD *= 1.0f - metalic;	  
		vec3 ambient = (kD * diffuse + specular) * vec3(occlusion);
		// Ambient part
		fragcolor = (ambient + Lo);
		fragcolor += emission * 2.0f;
	}
	else
	{
		fragcolor = albedo;
	}

	float brightness = dot(fragcolor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > ubo.bloom_threshold)
        outBrightness = vec4(fragcolor.rgb, 1.0) * mask;
    else
        outBrightness = vec4(0.0, 0.0, 0.0, 1.0);
   
  	outFragcolor = vec4(fragcolor, 1.0f );	
}