#version 450

layout (binding = 0) uniform sampler2D samplerBrightness;

layout(push_constant) uniform FDownsampleUbo 
{
	float scale;
} ubo;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

void main()
{
    vec2 pixeloffset = vec2(1.0f/1024.0f, 1.0f/720.0f);      
    vec3 downscalecolor = vec3(0.0f);
    downscalecolor += texture(samplerBrightness, vec2(inUV.x - 4.0f * pixeloffset.x, inUV.y)).xyz;         
    downscalecolor += texture(samplerBrightness, vec2(inUV.x + 4.0f * pixeloffset.x, inUV.y)).xyz;         
    downscalecolor += texture(samplerBrightness, vec2(inUV.x, inUV.y - 4.0f * pixeloffset.y)).xyz;         
    downscalecolor += texture(samplerBrightness, vec2(inUV.x, inUV.y + 4.0f * pixeloffset.y)).xyz;         
    downscalecolor *= 0.25f;
    outColor = vec4(downscalecolor, 1.0f);
}