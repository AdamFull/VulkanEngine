#version 450

//#define DEBUG
#ifdef DEBUG
#define CASCADES_COUNT 4
#endif

layout (triangles, invocations = CASCADES_COUNT) in;
layout (triangle_strip, max_vertices = 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 cascadeViewProjMat[CASCADES_COUNT];
} uboshadow;

void main() 
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Layer = gl_InvocationID;
		gl_Position = uboshadow.cascadeViewProjMat[gl_InvocationID] * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}