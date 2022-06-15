#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#ifdef HAS_BASECOLORMAP
layout(binding = 1) uniform sampler2D color_tex;
#endif

#ifdef HAS_METALLIC_ROUGHNESS
layout(binding = 2) uniform sampler2D rmah_tex;
#endif

#ifdef HAS_NORMALMAP
layout(binding = 3) uniform sampler2D normal_tex;
#endif

#ifdef HAS_OCCLUSIONMAP
layout(binding = 4) uniform sampler2D occlusion_tex;
#endif

#ifdef HAS_EMISSIVEMAP
layout(binding = 5) uniform sampler2D emissive_tex;
#endif

#ifdef HAS_HEIGHTMAP
layout(binding = 6) uniform sampler2D height_tex;
#endif

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inPosition;
layout(location = 3) in mat3 inTBN;

layout(location = 0) out uvec4 outPack;
layout(location = 1) out vec4 outEmissive;

#include "../shader_util.glsl"

layout(std140, binding = 0) uniform FUniformData 
{
  	mat4 model;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
} data;

layout(push_constant) uniform UBOMaterial
{
	float alphaCutoff;
	vec3 emissiveFactor;
	float normalScale;
	float occlusionStrenth;
	vec4 baseColorFactor;
	float metallicFactor;
	float roughnessFactor;
} ubo;

const float minRoughness = 0.04;

void main() 
{
	float alpha = ubo.alphaCutoff;
//BASECOLOR
	vec3 albedo_map = vec3(0.0);
#ifdef HAS_BASECOLORMAP
	albedo_map = texture(color_tex, inUV).rgb * ubo.baseColorFactor.rgb;
#else
	albedo_map = ubo.baseColorFactor.rgb;
#endif
	albedo_map *= inColor;

//METALLIC ROUGHNESS
	vec4 pbr_map = vec4(0.0);
	float roughness = ubo.roughnessFactor;
	float metallic = ubo.metallicFactor;
#ifdef HAS_METALLIC_ROUGHNESS
	vec4 metalRough = texture(rmah_tex, inUV);
	roughness = roughness * metalRough.g;
	metallic = metallic * pbr_map.b;
#endif
	roughness = clamp(roughness, minRoughness, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
	pbr_map = vec4(roughness, metallic, 0.0, 0.0);

//NORMALS
	mat3 normal = mat3(data.normal);
#ifndef HAS_TANGENTS
    vec3 pos_dx = dFdx(inPosition);
    vec3 pos_dy = dFdy(inPosition);
    vec3 tex_dx = dFdx(vec3(inUV, 0.0));
    vec3 tex_dy = dFdy(vec3(inUV, 0.0));
    vec3 t = normal * (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

#ifdef HAS_NORMALS
    vec3 ng = normalize(normal * inNormal);
#else
    vec3 ng = cross(pos_dx, pos_dy);
#endif

    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);
#else // HAS_TANGENTS
    mat3 tbn = inTBN;
#endif //END HAS_TANGENTS

	vec3 normal_map = vec3(0.0);
#ifdef HAS_NORMALMAP
	normal_map = getTangentSpaceNormalMap(normal_tex, tbn, inUV, ubo.normalScale);
#else
	normal_map = tbn[2].xyz;
#endif

	//normal_map *= (2.0 * float(gl_FrontFacing) - 1.0);

//AMBIENT OCCLUSION
	float ao = 1.0;
#ifdef HAS_OCCLUSIONMAP
	ao = texture(occlusion_tex, inUV).r;
    albedo_map = mix(albedo_map, albedo_map * ao, ubo.occlusionStrenth);
#endif

//EMISSION
	vec4 emission = vec4(0.0);
#ifdef HAS_EMISSIVEMAP
    emission = vec4(texture(emissive_tex, inUV).rgb * ubo.emissiveFactor, 1.0);
#endif

	outPack = packTextures(normal_map, albedo_map, pbr_map);
	outEmissive = emission;
}