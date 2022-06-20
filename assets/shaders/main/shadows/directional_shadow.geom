#version 450
#define LIGHT_COUNT 16

layout (triangles, invocations = LIGHT_COUNT) in;
layout (triangle_strip, max_vertices = 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 mvp[LIGHT_COUNT];
	int light_count;
} ubo;

void main() 
{
	for (int i = 0; i < ubo.light_count; i++)
	{
		gl_Layer = gl_InvocationID;
		vec4 tmpPos = gl_in[i].gl_Position;
		gl_Position = ubo.mvp[gl_InvocationID] * tmpPos;
		EmitVertex();
	}
	EndPrimitive();
}