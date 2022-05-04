#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;

layout(binding = 0) uniform FUniformData 
{
  mat4 model;
  mat4 view;
  mat4 projection;
  mat4 normal;
} ubo;

layout (location = 0) out vec3 outWorldPos;

void main() 
{
	outWorldPos = inPosition;
	// Convert cubemap coordinates into Vulkan coordinate space
	//outWorldPos.xy *= -1.0;
  mat4 viewModel = ubo.view * ubo.model;
  viewModel[3] = vec4(0.0, 0.0, 0.0, 1.0);
	gl_Position = ubo.projection * viewModel * vec4(inPosition, 1.0);
}