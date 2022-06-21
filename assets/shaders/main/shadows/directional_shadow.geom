#version 450
#define LIGHT_COUNT 16

layout (triangles, invocations = LIGHT_COUNT) in;
layout (triangle_strip, max_vertices = 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 mvp[LIGHT_COUNT];
} ubo;

void main() 
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Layer = gl_InvocationID;
		gl_Position = ubo.mvp[gl_InvocationID] * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}