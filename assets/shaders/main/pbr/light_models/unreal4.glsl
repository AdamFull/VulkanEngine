#include "../../../shared_shading.glsl"

vec3 evaluteUnreal4PBR(vec3 ambient, vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness) {

    float cosTheta_l = CosTheta(L, N);
    float cosTheta_v = CosTheta(V, N);

    // add lambertian diffuse term
    vec3 color = ambient / PI * cosTheta_l;

    vec3 wo = normalize(L);
    vec3 wi = normalize(V);

    float cosThetaO = AbsCosTheta(wo, N);
    float cosThetaI = AbsCosTheta(wi, N);
    vec3 wh = wi + wo;
    if (cosThetaI == 0 || cosThetaO == 0) return vec3(0);
    if (wh.x == 0 && wh.y == 0 && wh.z == 0) return vec3(0);
    wh = normalize(wh);

    float dotNH = clamp(dot(normalize(wh), normalize(N)), 0.0f, 1.0);
    float dotNL = clamp(dot(normalize(wo), normalize(N)), 0.0, 1.0);
    float dotNV = clamp(dot(normalize(wi), normalize(N)), 0.0, 1.0);

    float D = D_GGX(dotNH, roughness);
    float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
    float cosTheta = clamp(dot(wi, wh), 0.0f, 1.0f);
    vec3 F = F0 + (1.0 - F0) * pow(2, (-5.55473 * cosTheta - 6.98316) * cosTheta);

    // add specular term  
    if (cosTheta_l > 0 && cosTheta_v > 0) {
        vec3 kD = (vec3(1.0f) - F) * (1.0f - metallic);	
        color += evaluateCookTorrenceSpecularBRDF(D, G, F, cosTheta_l, cosTheta_v) * cosTheta_l;
    }

    return color;

}