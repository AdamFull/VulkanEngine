vec3 getTangentSpaceNormalMap(sampler2D samplerNormal, mat3 TBN, vec2 uv, float scale)
{
	vec3 normalColor = texture(samplerNormal, uv).rgb;
    normalColor = normalize(TBN * ((2.0 * normalColor - 1.0) * vec3(scale, scale, 1.0)));
	return normalColor;
}

// Calculate normal in tangent space
mat3 calculateTBN(vec3 normal, vec4 tangent)
{
	vec3 N = normalize(normal);
	vec3 T = normalize(tangent.xyz);
	vec3 B = normalize(cross(N, T) * tangent.w);
	
	return mat3(T, B, N);
}

vec3 getPositionFromDepth(vec2 texcoord, float depth, mat4 invViewProjection)
{
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = texcoord * 2.0f - 1.0f;
    clipSpaceLocation.z = depth;
    clipSpaceLocation.w = 1.0f;
    vec4 homogenousLocation = invViewProjection * clipSpaceLocation;
    return homogenousLocation.xyz / homogenousLocation.w;
}

vec3 WorldPosFromDepth(vec2 texcoord, float depth, mat4 invProjMatrix, mat4 invViewMatrix) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texcoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjMatrix * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = invViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

uvec4 packTextures(vec3 normal_map, vec3 albedo_map, vec4 pbr_map)
{
    uvec4 texture_pack;
    //Storing RG components of normal map in 32bit R component
    texture_pack.r = packHalf2x16(normal_map.rg);
    //Storing B component of normal map and R component of albedo in G
    texture_pack.g = packHalf2x16(vec2(normal_map.b, albedo_map.r));
    //Storing GB components of albedo in B
    texture_pack.b = packHalf2x16(albedo_map.gb);
    // Store metal, roughness, ao and heignt in A component
    texture_pack.a = packUnorm4x8(pbr_map);
    return texture_pack;
}

void unpackTextures(in uvec4 texture_pack, out vec3 normal_map, out vec3 albedo_map, out vec4 pbr_map)
{
    vec2 pack_R = unpackHalf2x16(texture_pack.r);
	vec2 pack_G = unpackHalf2x16(texture_pack.g);
	vec2 pack_B = unpackHalf2x16(texture_pack.b);
	pbr_map = unpackUnorm4x8(texture_pack.a);
    normal_map = vec3(pack_R, pack_G.r);
    albedo_map = vec3(pack_G.g, pack_B);
}