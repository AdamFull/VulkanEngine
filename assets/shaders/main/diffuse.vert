#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outColor;
#ifdef HAS_NORMALS
layout(location = 2) out vec3 outNormal;
#endif
#ifdef HAS_TANGENTS
layout (location = 3) out vec4 outTangent;
#endif

#include "../shader_util.glsl"

void main() 
{
	outUV = inTexCoord * 1.0;
  	outColor = inColor;
#ifdef HAS_NORMALS
	outNormal = inNormal;
#endif
#ifdef HAS_TANGENTS
	outTangent = inTangent;
#endif  
  	//ubo.projection * ubo.view * ubo.model * tmpPos;
	gl_Position = vec4(inPosition, 1.0);
}