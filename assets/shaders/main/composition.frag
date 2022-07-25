#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#include "../shader_util.glsl"

#ifndef FXAA_REDUCE_MIN
    #define FXAA_REDUCE_MIN   (1.0/ 16.0)
#endif
#ifndef FXAA_REDUCE_MUL
    #define FXAA_REDUCE_MUL   (1.0 / 8.0)
#endif
#ifndef FXAA_SPAN_MAX
    #define FXAA_SPAN_MAX     8.0
#endif

layout (binding = 0) uniform sampler2D samplerColor;
layout (binding = 1) uniform sampler2D velocity_tex;
layout (binding = 2) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform FBloomUbo
{
    //HDR
    float gamma;
	float exposure;
    bool enableFXAA;
    float lumaThreshold;
    float reduceMin;
    vec2 texelStep;
} ubo;

layout(std140, binding = 3) uniform UBOMotionBlur
{
	float velocityScale;
} motion;

#define MAX_SAMPLES 30

vec2 flippedUV(vec2 uv)
{
    return vec2(uv.s, uv.t * -1.0);
}

void main() 
{
    vec4 motionBlur = per_object_motion_blir(velocity_tex, samplerColor, inUV, motion.velocityScale, MAX_SAMPLES);

    //vec3 fragcolor = texture(samplerColor, inUV).rgb;
    vec3 fragcolor = vec3(0.0);
    if(ubo.enableFXAA)
        fragcolor = fxaa(samplerColor, inUV, ubo.lumaThreshold, ubo.reduceMin, FXAA_REDUCE_MUL, FXAA_SPAN_MAX, ubo.texelStep).rgb;
    else
        fragcolor = texture(samplerColor, inUV).rgb;

    fragcolor = motionBlur.rgb;
    vec3 brightness = texture(samplerBrightness, inUV).rgb;
    //fragcolor = mix(fragcolor, brightness, 0.4);
    //Exposure
    fragcolor = Uncharted2Tonemap(fragcolor * ubo.exposure);
    //Gamma correction
    fragcolor = fragcolor * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
    fragcolor = pow(fragcolor, vec3(1.0f / ubo.gamma));
	outColor = vec4(fragcolor, 1.0);
}