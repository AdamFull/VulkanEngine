
#include "../../lightning_base.glsl"

//Based on https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
#ifndef OMNI_SHADOWS
#define OMNI_SHADOWS

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float omniShadowProjection(samplerCubeArray shadomwap_tex, vec3 P, vec3 offset, float bias, int layer)
{
	float shadow = 1.0;

	float closestDist = texture(shadomwap_tex, vec4(P + offset, layer)).r * 25.f;
	float currentDist = length(P);
	if (currentDist -  bias > closestDist) 
		shadow = 0.0;

	return shadow;
}

float omniShadowFilterPCF(samplerCubeArray shadomwap_tex, vec3 P, float viewDistance, float bias, int layer)
{
	float far_plane = 25.0;
	float shadowFactor = 0.0;
	int samples  = 20;
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0; 
	for(int i = 0; i < samples; ++i)
		shadowFactor += omniShadowProjection(shadomwap_tex, P, sampleOffsetDirections[i] * diskRadius, bias, layer);
	shadowFactor /= float(samples); 
	return shadowFactor;
}

float getOmniShadow(samplerCubeArray shadomwap_tex, vec3 fragPos, vec3 viewPos, vec3 N, FPointLight light, int layer, bool enablePCF) 
{
    float shadow = 1.0;
	vec3 shadowClip = fragPos - light.position;
	float viewDistance = length(viewPos - fragPos);
	float bias = 0.15;

	if (enablePCF)
		shadow = omniShadowFilterPCF(shadomwap_tex, shadowClip, viewDistance, bias, layer);
	else
		shadow = omniShadowProjection(shadomwap_tex, shadowClip, vec3(0.0), bias, layer);

	return shadow;
}

#endif