vec3 getPositionFromDepth(vec2 texcoord, float depth, mat4 invViewProjection)
{
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = texcoord * 2.0f - 1.0f;
    clipSpaceLocation.z = depth;
    clipSpaceLocation.w = 1.0f;
    vec4 homogenousLocation = invViewProjection * clipSpaceLocation;
    return homogenousLocation.xyz / homogenousLocation.w;
}

vec2 encodeSpheremapTransform(vec3 normal)
{
    return normalize(normal.xy) * sqrt(normal.z * 0.5 + 0.5);
} 

vec3 decodeSpheremapTransform(vec2 spheremap)
{
    vec3 normal;
    normal.z = length(spheremap.xy) * 2 - 1;
    normal.xy = normalize(spheremap.xy) * sqrt(1 - normal.z * normal.z);
    return normal;
}