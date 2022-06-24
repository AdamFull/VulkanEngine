#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

#include "../shared_lightning.glsl"
#include "../shader_util.glsl"

#define SHADOW_MAP_CASCADE_COUNT 4

layout (constant_id = 0) const int testConstant = 0;

layout (binding = 0) uniform sampler2D brdflut_tex;
layout (binding = 1) uniform samplerCube irradiance_tex;
layout (binding = 2) uniform samplerCube prefiltred_tex;

layout (input_attachment_index = 0, binding = 3) uniform usubpassInput packed_tex;
layout (input_attachment_index = 1, binding = 4) uniform subpassInput emission_tex;
//layout (input_attachment_index = 2, binding = 5) uniform subpassInput position_tex;
layout (input_attachment_index = 2, binding = 5) uniform subpassInput depth_tex;
layout (binding = 7) uniform sampler2DArray shadowmap_tex;
//layout (binding = 7) uniform sampler2DArray shadowArr;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragcolor;

layout(std140, binding = 12) uniform UBODeferred
{
	mat4 invViewProjection;
	mat4 view;
	vec4 viewPos;
	int lightCount;
} ubo;

layout(std140, binding = 13) uniform UBODebug
{
	int target;
	int cascade;
} debug;

//Lights
layout(std140, binding = 14) uniform UBOLights
{
	FLight lights[32];
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

	vec3 emission = subpassLoad(emission_tex).rgb;

	//albedo = pow(albedo, vec3(2.2f));

	float roughness = mrah.r;
	float metallic = mrah.g;

	float alphaRoughness = roughness;
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
		F0 = mix(F0, albedo, metallic);

		// Light contribution
		vec3 Lo = vec3(0.0f);
		for(int i = 0; i < ubo.lightCount; i++) 
		{
			FLight light = lights.lights[i];
			float shadow_factor = 0.0;
			vec3 L = vec3(0.0);
			float atten = 1.0;

			// Point light
			if(light.type == 0)
			{
				L = light.position - inWorldPos;
				float dist = length(L);
				L = normalize(L);
				atten = clamp(1.0 - pow(dist, 2.0f)/pow(light.intencity, 2.0f), 0.0f, 1.0f); atten *= atten;
			}

			// Directional light
			if(light.type == 1)
			{
				L = -light.direction;
				float dist = length(L);
				L = normalize(L);
				atten = light.intencity;
			}

			// Spot light
			if(light.type == 2)
			{
				float spotAtten = 1.0;
				L = normalize(light.position - inWorldPos);
				float dist = length(L);
				L = normalize(L);
				atten = clamp(1.0 - pow(dist, 2.0f)/pow(light.intencity, 2.0f), 0.0f, 1.0f); atten *= atten;
				
				float theta = dot(-L, normalize(light.direction));
				float epsilon = light.outerConeAngle - light.innerConeAngle;
				spotAtten = clamp((theta - light.innerConeAngle) / epsilon, 0.0, 1.0);
				atten *= spotAtten;
			}
			Lo += atten * light.color * specularContribution(albedo, L, V, normal, F0, metallic, alphaRoughness);
		}

		vec2 brdf = texture(brdflut_tex, vec2(max(dot(normal, V), 0.0f), roughness)).rg;
		//vec3 reflection = prefilteredReflection(R, roughness, prefiltred_tex);
		vec3 reflection = textureLod(prefiltred_tex, R,  roughness * 9.0).rgb;
		vec3 irradiance = pow(texture(irradiance_tex, normal).rgb, vec3(2.2f));

		vec3 F = F_SchlickR(max(dot(normal, V), 0.0f), F0, roughness);
		vec3 kD = 1.0f - F;

		vec3 diffuseBrdf = albedo * (1.0 - metallic);

		// Diffuse based on irradiance
		vec3 diffuse = kD * diffuseBrdf * irradiance;
		vec3 specular = reflection * (F * brdf.x + brdf.y);

		// Ambient part
		fragcolor = diffuse + (emission * 4.0) + specular + Lo;
	}
	else
	{
		fragcolor = albedo;
	}

	//TODO: remove skybox from shadow render
	if(debug.target == 0)
		fragcolor = fragcolor;
	else if(debug.target == 1)
		fragcolor = vec3(depth);
	else if(debug.target == 2)
		fragcolor = inWorldPos;
	else if(debug.target == 3)
		fragcolor = normal;
	else if(debug.target == 4)
		fragcolor = albedo;
	else if(debug.target == 5)
		fragcolor = vec3(metallic);
	else if(debug.target == 6)
		fragcolor = vec3(roughness);
	else if(debug.target == 7)
		fragcolor = texture(shadowmap_tex, vec3(inUV, debug.cascade)).rrr;
	else if(debug.target == 8 || debug.target == 9 || debug.target == 10)
	{
		vec3 viewPos = (ubo.view * vec4(inWorldPos, 1.0)).xyz;
		float shadow_factor = 1.0;
		FLight light;
		
		for(int i = 0; i < ubo.lightCount; i++) 
		{
			light = lights.lights[i];
			shadow_factor = getCascadeShadow(shadowmap_tex, viewPos, inWorldPos, normal, light);
		}

		if(debug.target == 8)
			fragcolor = vec3(1.0) * shadow_factor;
		else if(debug.target == 9)
			fragcolor = fragcolor * shadow_factor;
		else if(debug.target == 10)
		{
			uint cascadeIndex = 0;
			for(uint i = 0; i < 4 - 1; ++i) {
				if(viewPos.z < light.cascadeSplits[i]) {	
					cascadeIndex = i + 1;
					break;
				}
			}

			switch(cascadeIndex) 
			{
			case 0 : 
				fragcolor *= vec3(1.0f, 0.25f, 0.25f);
				break;
			case 1 : 
				fragcolor *= vec3(0.25f, 1.0f, 0.25f);
				break;
			case 2 : 
				fragcolor *= vec3(0.25f, 0.25f, 1.0f);
				break;
			case 3 : 
				fragcolor *= vec3(1.0f, 1.0f, 0.25f);
				break;
			}
		}
	}
   
  	outFragcolor = vec4(fragcolor, 1.0);
}