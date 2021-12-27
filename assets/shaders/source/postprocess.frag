#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout (set = 1, binding = 0) uniform sampler2D samplerColor;
layout (set = 1, binding = 1) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(std140, set = 0, binding = 0) uniform UBO 
{
    //HDR
    float gamma;
	float exposure;

    //Bloom
	float blurScale;
    float blurStrength;
    int direction;
} ubo;

const float weights[] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main() 
{
    vec2 tex_offset = 1.0 / textureSize(samplerBrightness, 0) * ubo.blurScale; // gets size of single texel
	vec3 result = texture(samplerBrightness, inUV).rgb * weights[0]; // current fragment's contribution

    for(int i = 1; i < weights.length(); ++i)
	{
		//if (ubo.direction == 1)
		{
			// H
			result += texture(samplerBrightness, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weights[i] * ubo.blurStrength;
			result += texture(samplerBrightness, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weights[i] * ubo.blurStrength;
		//}
		//else
		//{
			// V
			result += texture(samplerBrightness, inUV + vec2(0.0, tex_offset.y * i)).rgb * weights[i] * ubo.blurStrength;
			result += texture(samplerBrightness, inUV - vec2(0.0, tex_offset.y * i)).rgb * weights[i] * ubo.blurStrength;
		}
	}

    float rOffset = 0.01;
    float gOffset = 0.02;
    float bOffset = -0.01;

    vec4 rValue = texture(samplerColor, inUV - rOffset);  
    vec4 gValue = texture(samplerColor, inUV - gOffset);
    vec4 bValue = texture(samplerColor, inUV - bOffset); 

    vec3 color = vec3(rValue.r, gValue.g, bValue.b);

    vec3 fragcolor = color + result;
    //Exposure
    fragcolor = vec3(1.0) - exp(-fragcolor * ubo.exposure);
    //Gamma correction
    fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
	outColor = vec4(fragcolor, 1.0);
}