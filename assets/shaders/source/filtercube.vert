#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec4 inJoint;
layout(location = 6) in vec4 inWeights;

layout(push_constant) uniform PushConsts 
{
	layout (offset = 0) mat4 mvp;
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