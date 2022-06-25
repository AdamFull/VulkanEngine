#version 460

//#define DEBUG
#ifdef DEBUG
#define INVOCATION_COUNT 4
#endif

#define NUM_FACES 6

layout (triangles, invocations = INVOCATION_COUNT) in;
layout (triangle_strip, max_vertices= NUM_FACES * 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4[NUM_FACES] viewProjMat[INVOCATION_COUNT];
	int passedLights;
} uboshadow;

void main() 
{
    if(gl_InvocationID > uboshadow.passedLights)
		return;

	for(int face = 0; face < NUM_FACES; face++) 
    {
		gl_Layer = face;
		for(int vertex_index = 0; vertex_index < 3; vertex_index++) 
        {
            gl_Layer = gl_InvocationID;
			gl_Position = uboshadow.viewProjMat[gl_InvocationID][face] * gl_in[vertex_index].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}