#version 450

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in uint inColor;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

layout(binding = 0) uniform FUniformDataUI 
{
  vec2 scale;
	vec2 translate;
} ubo;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	outUV = inUV;
	//outColor = vec4(inColor & uint(0xFF000000), inColor & uint(0x00FF0000), inColor & uint(0x0000FF00), inColor & uint(0x000000FF));
	outColor = unpackUnorm4x8(inColor);
	gl_Position = vec4(inPos * ubo.scale + ubo.translate, 0.0, 1.0);
}