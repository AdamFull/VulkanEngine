#version 450
#extension GL_EXT_scalar_block_layout : enable
#define PI 3.1415926538

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
	float ambient;
	float tone;
	float gamma;
} ubo;

const vec3 Fdielectric = vec3(0.04);

// Normal distribution function
float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

// Used by method below
float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;
}

// Normal distribution function. Describes self-shadowing of microfacets. When a surface is very rough,
// microfacets can overshadow other microfacets which reduces reflected light.
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggxL = geometrySchlickGGX(NdotL, roughness);
    float ggxV = geometrySchlickGGX(NdotV, roughness);

    return ggxL * ggxV;
}

// Describes the ratio of surface reflection at different surface angles.
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

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

void main() 
{
	// Get G-Buffer values
	vec3 fragPos = texture(position_tex, inUV).rgb;
	float mask = texture(lightning_mask_tex, inUV).r;
	vec3 normal = texture(normal_tex, inUV).rgb;
	vec4 albedo = texture(albedo_tex, inUV);
	vec4 emission = texture(emission_tex, inUV);
	vec4 mrah = texture(mrah_tex, inUV);

	float metalic = mrah.r;
	float roughness = mrah.g;
	float occlusion = mrah.b;
	float height = mrah.a;

	// Calculate direction from fragment to viewPosition
    vec3 V = normalize(ubo.viewPos - fragPos);

	// Reflectance at normal incidence angle
    vec3 F0 = mix(Fdielectric, albedo.rgb, metalic);

	// Reflection vector
    vec3 R = reflect(-V, normal);

    // Light contribution
    vec3 Lo = vec3(0.0, 0.0, 0.0);

	//outFragcolor = vec4(mrah.rgb, 1.0);
	//return;
	
	for(int i = 0; i < ubo.lightCount * mask; ++i)
	{
		vec3 L = normalize(ubo.lights[i].position - fragPos);
        vec3 H = normalize(V + L);
        float dist = length(ubo.lights[i].position - fragPos);

		float attenuation = 1.0 / (dist * dist);
        vec3 radiance = ubo.lights[i].color * attenuation;

		// BRDF
        float NDF = distributionGGX(normal, H, roughness);
        float G = geometrySmith(normal, V, L, roughness);
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0);
        vec3 specular = nominator / max(denominator, 0.001);

		vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metalic;

        float NdotL = max(dot(normal, L), 0.0);

        Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
	}

	// Calculate ambient lighting from IBL
    vec3 F = fresnelSchlickRoughness(max(dot(normal, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metalic;

	vec3 irradiance = texture(irradiance_tex, normal).rgb;
    vec3 diffuse = albedo.rgb * irradiance;

	const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefiltred_tex, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdflut_tex, vec2(max(dot(normal, V), 0.0), roughness)).rg;
    vec3 reflection = prefilteredColor * (kS * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + reflection * vec3(1.5));

	// Ambient part
	//vec3 fragcolor = albedo.rgb * pow(ambient, mask);
	float inv_mask = pow(mask, 0);
	vec3 fragcolor = ((ambient + Lo)*mask + albedo.rgb*inv_mask) + emission.rgb;
	//vec3 fragcolor = ambient + Lo;

	// Tone mapping
	fragcolor = Uncharted2Tonemap(fragcolor * ubo.tone);
	fragcolor = fragcolor * (1.0 / Uncharted2Tonemap(vec3(11.2)));	
	// Gamma correction
	fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
   
  	outFragcolor = vec4(fragcolor, 1.0 );	
}