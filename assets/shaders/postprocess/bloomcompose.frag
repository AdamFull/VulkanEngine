#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 1, binding = 0) uniform sampler2D samplerColor;
layout (set = 1, binding = 1) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(std140, set = 0, binding = 0) uniform FBloomUbo 
{
    //HDR
    float gamma;
	float exposure;
} ubo;

void main() 
{
    vec3 fragcolor = texture(samplerColor, inUV).rgb + texture(samplerBrightness, inUV).rgb;
    //Exposure
    fragcolor = vec3(1.0) - exp(-fragcolor * ubo.exposure);
    //Gamma correction
    fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
	outColor = vec4(fragcolor, 1.0);
}