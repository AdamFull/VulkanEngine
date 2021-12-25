#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;
layout (constant_id = 0) const uint NUM_SAMPLES = 1024u;

#include "shared_lightning.glsl"

vec2 BRDF(float NoV, float roughness)
{
	// Normal always points along z-axis for the 2D lookup 
	const vec3 N = vec3(0.0f, 0.0f, 1.0f);
	vec3 V = vec3(sqrt(1.0f - NoV * NoV), 0.0f, NoV);

	vec2 LUT = vec2(0.0f);

	for (uint i = 0u; i < NUM_SAMPLES; i++) {
		vec2 Xi = hammersley2d(i, NUM_SAMPLES);
		vec3 H = importanceSample_GGX(Xi, roughness, N);
		vec3 L = 2.0f * dot(V, H) * H - V;

		float dotNL = max(dot(N, L), 0.0f);
		float dotNV = max(dot(N, V), 0.0f);
		float dotVH = max(dot(V, H), 0.0f); 
		float dotNH = max(dot(H, N), 0.0f);

		if (dotNL > 0.0f) {
			float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
			float G_Vis = (G * dotVH) / (dotNH * dotNV);
			float Fc = pow(1.0f - dotVH, 5.0f);
			LUT += vec2((1.0f - Fc) * G_Vis, Fc * G_Vis);
		}
	}

	return LUT / float(NUM_SAMPLES);
}

void main() 
{
	outColor = vec4(BRDF(inUV.s, 1.0f-inUV.t), 0.0f, 1.0f);
}