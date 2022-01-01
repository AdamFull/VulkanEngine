#version 450
layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform PushConsts 
{
	layout (offset = 0) mat4 mvp;
	layout (offset = 64) float deltaPhi;
	layout (offset = 68) float deltaTheta;
} pushConsts;

layout (location = 0) out vec3 outUVW;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	outUVW = inPosition;
	gl_Position = pushConsts.mvp * vec4(inPosition.xyz, 1.0);
}