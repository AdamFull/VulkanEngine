#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout (binding = 0) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(std140, binding = 1) uniform FBloomUbo 
{
    //Bloom
	float blurScale;
    float blurStrength;
    int direction;
} ubo;

#include "../shared_shaders.glsl"

/*const float weights[] = float[](
0.0024499299678342, 0.0043538453346397, 0.0073599963704157, 0.0118349786570722, 0.0181026699707781,
0.0263392293891488, 0.0364543006660986, 0.0479932050577658, 0.0601029809166942, 0.0715974486241365,
0.0811305381519717, 0.0874493212267511, 0.0896631113333857, 0.0874493212267511, 0.0811305381519717,
0.0715974486241365, 0.0601029809166942, 0.0479932050577658, 0.0364543006660986, 0.0263392293891488,
0.0181026699707781, 0.0118349786570722, 0.0073599963704157, 0.0043538453346397, 0.0024499299678342);*/
const float weights[] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() 
{
    vec2 tex_offset = 1.0 / textureSize(samplerColor, 0) * ubo.blurScale; // gets size of single texel
	vec3 result = texture(samplerColor, inUV).rgb * weights[0]; // current fragment's contribution

	for(int i = 1; i < weights.length(); ++i)
	{
		if (ubo.direction == 1)
		{
			// H
			result += texture(samplerColor, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weights[i] * ubo.blurStrength;
			result += texture(samplerColor, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weights[i] * ubo.blurStrength;
		}
		else
		{
			// V
			result += texture(samplerColor, inUV + vec2(0.0, tex_offset.y * i)).rgb * weights[i] * ubo.blurStrength;
			result += texture(samplerColor, inUV - vec2(0.0, tex_offset.y * i)).rgb * weights[i] * ubo.blurStrength;
		}
	}

	outColor = vec4(result, 1.0);
}