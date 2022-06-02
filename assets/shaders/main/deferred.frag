#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

#include "../shared_lightning.glsl"
#include "../shader_util.glsl"

layout (binding = 0) uniform sampler2D brdflut_tex;
layout (binding = 1) uniform samplerCube irradiance_tex;
layout (binding = 2) uniform samplerCube prefiltred_tex;

layout (input_attachment_index = 0, binding = 3) uniform usubpassInput packed_tex;
layout (input_attachment_index = 1, binding = 4) uniform subpassInput emission_tex;
layout (input_attachment_index = 2, binding = 5) uniform subpassInput depth_tex;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragcolor;

layout(push_constant) uniform UBOLightning
{
	mat4 invViewProjection;
	vec4 viewPos;
	int pointLightCount;
	int directionalLightCount;
	int spotLightCount;
} ubo;

//Lights
layout(std140, binding = 9) uniform UBOLights
{
	PointLight pointLights[32];
	DirectionalLight directionalLights[32];
	SpotLight spotLights[32];
} lights;

void main() 
{
	// ---Get G-Buffer values---

	//Load depth and world position
	float depth = subpassLoad(depth_tex).r;
	vec3 inWorldPos = getPositionFromDepth(inUV, depth, ubo.invViewProjection);

	vec3 normal = vec3(0.0);
	vec3 albedo = vec3(0.0);
	vec4 mrah = vec4(0.0);

	// Loading texture pack
	uvec4 packed_data = subpassLoad(packed_tex);
	unpackTextures(packed_data, normal, albedo, mrah);

	albedo = pow(albedo, vec3(2.2f));
	vec3 emission = pow(subpassLoad(emission_tex).rgb, vec3(2.2f));

	float metalic = mrah.r;
	float roughness = mrah.g;
	float occlusion = mrah.b;
	float height = mrah.a;

	bool calculateLightning = normal != vec3(0.0f);

	vec3 fragcolor = vec3(0.0f);
	if(calculateLightning)
	{
		vec3 cameraPos = ubo.viewPos.xyz;
		// Calculate direction from fragment to viewPosition
		vec3 V = normalize(cameraPos - inWorldPos);
		// Reflection vector
		vec3 R = reflect(-V, normal);

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
			Lo += atten * light.color.rgb * light.intencity * specularContribution(albedo, L, V, normal, F0, metalic, roughness);
		}

		//Adding directional lights
		for(int i = 0; i < ubo.directionalLightCount; i++) 
		{
			DirectionalLight light = lights.directionalLights[i];
			vec3 L = -light.direction;
			float dist = length(L);
			L = normalize(L);
			Lo += light.color.rgb * light.intencity * specularContribution(albedo, L, V, normal, F0, metalic, roughness);
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
				Lo += light.color.rgb * atten * light.intencity * specularContribution(albedo, L, V, normal, F0, metalic, roughness);
			}
		}

		vec2 brdf = texture(brdflut_tex, vec2(max(dot(normal, V), 0.0f), roughness)).rg;
		vec3 reflection = prefilteredReflection(R, roughness, prefiltred_tex);	
		vec3 irradiance = pow(texture(irradiance_tex, normal).rgb, vec3(2.2f));

		// Diffuse based on irradiance
		vec3 diffuse = irradiance * albedo;	

		vec3 F = F_SchlickR(max(dot(normal, V), 0.0f), F0, roughness);

		// Specular reflectance
		vec3 specular = reflection * (F * brdf.r + brdf.g);

		// Ambient part
		vec3 kD = 1.0f - F;
		kD *= 1.0f - metalic;	  
		vec3 ambient = (kD * diffuse + specular); //* vec3(occlusion);
		// Ambient part
		fragcolor = (ambient + Lo);
		fragcolor += emission * 2.0f;
	}
	else
	{
		fragcolor = albedo;
	}
   
  	outFragcolor = vec4(fragcolor, 1.0);	
}