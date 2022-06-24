#include "../../../shared_shading.glsl"

// source: https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf

float FrostbiteDiffuse(float NdotV, float NdotL, float LdotH, float roughness) 
{
    float energyBias = mix(0.0, 0.5, roughness);
    float energyFactor = mix(1.0, 1.0 / 1.51, roughness);
    float fd90 = energyBias + 2.0 * LdotH * LdotH * roughness;
    vec3 f0 = vec3(1.0f, 1.0f, 1.0f);
    float lightScatter = F_Schlick(NdotL, fd90, f0).r;
    float viewScatter = F_Schlick(NdotL, fd90, f0).r;
    
    return lightScatter * viewScatter * energyFactor;
}

float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{        
    // This is the optimize version
    float alphaG2 = alphaG * alphaG;
    // Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
    float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
    float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);
    return 0.5f / (Lambda_GGXV + Lambda_GGXL);
}

vec3 evaluateFrostbitePBR(vec3 ambient, vec3 L, vec3 V, vec3 N, float metallic, float roughness) 
{
    float NdotV = abs(dot(N, V)) + 1e-5f; // avoid artifact
    vec3 H = normalize(V + L);
    float LdotH = clamp(dot(L, H),0.0f, 1.0f);
    float NdotH = clamp(dot(N, H),0.0f, 1.0f);
    float NdotL = clamp(dot(N, L),0.0f, 1.0f);

    // add lambertian diffuse term vec3(0.f);// 
    vec3 color = FrostbiteDiffuse(NdotV, NdotL, LdotH, roughness) * (ambient / PI) * NdotL;
    //
    if (LdotH > 0 && NdotH > 0 && NdotL > 0) {

        // Specular BRDF
        // renge [0,1] from non-/to metallic
        float reflectence = 0.6;
        vec3 f0 = vec3(0.16f * reflectence * reflectence);
        // lets assume that at 90 degrees everything will be reflected
        vec3 F = F_Schlick(LdotH, 1.0, f0);
        float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
        float D = D_GGX(NdotH, roughness);
        vec3 kD = (vec3(1.0f) - F) * (1.0f - metallic);	
        color += (1.f/PI) * evaluateCookTorrenceSpecularBRDF(D, Vis, F, NdotL, NdotV) * NdotL;
    }

    return color;
}