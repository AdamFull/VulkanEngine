//From https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
vec2 ParallaxMapping(sampler2D height_map, vec2 texCoords, vec3 viewDir, float height_scale, float minLayers, float maxLayers)
{ 
	vec2  currentTexCoords = texCoords;
	float currentDepthMapValue = texture(height_map, inUV).r;

	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * height_scale; 
	vec2 deltaTexCoords = P / numLayers;

	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(height_map, inUV).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(height_map, inUV).r - currentLayerDepth + layerDepth;
		
	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;    
}

vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) 
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.3846153846) * direction;
	vec2 off2 = vec2(3.2307692308) * direction;
	color += texture(image, uv) * 0.2270270270;
	color += texture(image, uv + (off1 / resolution)) * 0.3162162162;
	color += texture(image, uv - (off1 / resolution)) * 0.3162162162;
	color += texture(image, uv + (off2 / resolution)) * 0.0702702703;
	color += texture(image, uv - (off2 / resolution)) * 0.0702702703;
	return color;
}

vec3 getNormals(sampler2D samplerNormal, vec3 position, vec3 normal, vec2 uv)
{
	vec3 tangentNormal = texture(samplerNormal, uv).rgb * 2.0f - 1.0f;

	vec3 q1 = dFdx(position);
	vec3 q2 = dFdy(position);
	vec2 st1 = dFdx(uv);
	vec2 st2 = dFdy(uv);

	vec3 N = normalize(inNormal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return TBN * tangentNormal;
}

vec3 getNormalsOld(sampler2D samplerNormal, vec3 normal, vec3 tangent, vec2 uv)
{
	vec3 tangentNormal = texture(samplerNormal, uv).xyz * 2.0 - vec3(1.0);
	// Calculate normal in tangent space
	vec3 N = normalize(normal);
	vec3 T = normalize(tangent);
	//T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	
	return TBN * tangentNormal;
}