#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#include "../shader_util.glsl"

layout (binding = 0) uniform sampler2D samplerColor;
layout (binding = 1) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform FBloomUbo
{
    //HDR
    float gamma;
	float exposure;
} ubo;

void main() 
{
    vec3 fragcolor = texture(samplerColor, inUV).rgb;
    vec3 brightness = texture(samplerBrightness, inUV).rgb;
    //fragcolor = mix(fragcolor, brightness, 0.4);
    //Exposure
    fragcolor = Uncharted2Tonemap(fragcolor * ubo.exposure);
    //Gamma correction
    fragcolor = fragcolor * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
    fragcolor = pow(fragcolor, vec3(1.0f / ubo.gamma));
	outColor = vec4(fragcolor, 1.0);
}