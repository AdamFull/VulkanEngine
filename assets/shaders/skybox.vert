#version 450

layout (location = 0) in vec3 inPosition;

layout(binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  vec4 viewPosition;
} ubo;

layout (location = 0) out vec3 outUVW;

void main() 
{
	outUVW = inPosition;
	// Convert cubemap coordinates into Vulkan coordinate space
	outUVW.xy *= -1.0;
	gl_Position = ubo.projection * ubo.model * vec4(inPosition.xyz, 1.0);
}