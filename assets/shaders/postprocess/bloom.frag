#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout (set = 1, binding = 0) uniform sampler2D samplerColor;
layout (set = 1, binding = 1) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(std140, set = 0, binding = 0) uniform FBloomUbo 
{
    //HDR
    float gamma;
	float exposure;
    //Bloom
	float blurScale;
    float blurStrength;
    int direction;
} ubo;

#include "../shared_shaders.glsl"

void main() 
{
	vec4 result = blur13(samplerBrightness, inUV, textureSize(samplerBrightness, 0), ubo.direction == 1 ? vec2(0.0, 1.0) : vec2(1.0, 0.0), ubo.blurStrength, ubo.blurScale);

    vec3 fragcolor = texture(samplerColor, inUV).rgb + result.rgb;
    //Exposure
    fragcolor = vec3(1.0) - exp(-fragcolor * ubo.exposure);
    //Gamma correction
    fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
	outColor = vec4(fragcolor, 1.0);
}