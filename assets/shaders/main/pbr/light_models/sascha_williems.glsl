#include "../../../shared_shading.glsl"

vec3 specularContribution(vec3 diffuse, vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize(L + V);
	float dotNH = clamp(dot(N, H), 0.0f, 1.0f);
	float dotNV = clamp(abs(dot(N, V)), 0.001f, 1.0f);
	float dotNL = clamp(dot(N, L), 0.001f, 1.0f);

	vec3 color = vec3(0.0f);

	if (dotNL > 0.0f) 
	{
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, 1.0, F0);
		vec3 spec = D * F * G / (4.0f * dotNL * dotNV);		
		vec3 kD = (vec3(1.0f) - F) * (1.0f - metallic);			
		color += ((kD * diffuse / PI) + spec) * dotNL;
	}

	return color;
}