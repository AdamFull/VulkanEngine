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

//Gaussian blurs https://github.com/Jam3/glsl-fast-gaussian-blur
vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction, float strength, float scale) 
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.3333333333333333) * direction;
	color += texture(image, uv) * 0.29411764705882354;
	color += texture(image, uv + (off1 / resolution * scale)) * 0.35294117647058826 * strength;
	color += texture(image, uv - (off1 / resolution * scale)) * 0.35294117647058826 * strength;
	return color; 
}

vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction, float strength, float scale) 
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.3846153846) * direction;
	vec2 off2 = vec2(3.2307692308) * direction;
	color += texture(image, uv) * 0.2270270270;
	color += texture(image, uv + (off1 / resolution * scale)) * 0.3162162162 * strength;
	color += texture(image, uv - (off1 / resolution * scale)) * 0.3162162162 * strength;
	color += texture(image, uv + (off2 / resolution * scale)) * 0.0702702703 * strength;
	color += texture(image, uv - (off2 / resolution * scale)) * 0.0702702703 * strength;
	return color;
}

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction, float strength, float scale) 
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.411764705882353) * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
	vec2 off3 = vec2(5.176470588235294) * direction;
	color += texture(image, uv) * 0.1964825501511404;
	color += texture(image, uv + (off1 / resolution * scale)) * 0.2969069646728344 * strength;
	color += texture(image, uv - (off1 / resolution * scale)) * 0.2969069646728344 * strength;
	color += texture(image, uv + (off2 / resolution * scale)) * 0.09447039785044732 * strength;
	color += texture(image, uv - (off2 / resolution * scale)) * 0.09447039785044732 * strength;
	color += texture(image, uv + (off3 / resolution * scale)) * 0.010381362401148057 * strength;
	color += texture(image, uv - (off3 / resolution * scale)) * 0.010381362401148057 * strength;
	return color;
}

vec3 getNormals(sampler2D samplerNormal, vec3 position, vec3 normal, vec2 uv)
{
	vec3 normalColor = texture(samplerNormal, uv).rgb;
	vec3 tangentNormal = normalColor * 2.0f - 1.0f;

	vec3 q1 = dFdx(position);
	vec3 q2 = dFdy(position);
	vec2 st1 = dFdx(uv);
	vec2 st2 = dFdy(uv);

	vec3 N = normalize(normal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

vec3 getNormalsOld(sampler2D samplerNormal, vec3 normal, vec4 tangent, vec2 uv)
{
	vec3 normalColor = texture(samplerNormal, uv).rgb;
	vec3 tangentNormal = normalColor * 2.0f - 1.0f;
	// Calculate normal in tangent space
	vec3 N = normalize(normal);
	vec3 T = normalize(tangent.xyz);
	vec3 B = normalize(cross(N, T) * tangent.w);
	mat3 TBN = mat3(T, B, N);
	
	return normalize(TBN * tangentNormal);
}