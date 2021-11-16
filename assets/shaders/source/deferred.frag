#version 450

layout (set = 1, binding = 0) uniform sampler2D position_tex;
layout (set = 1, binding = 1) uniform sampler2D lightning_mask_tex;
layout (set = 1, binding = 2) uniform sampler2D normal_tex;
layout (set = 1, binding = 3) uniform sampler2D albedo_tex;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragcolor;

struct Light {
	vec4 position;
	vec3 color;
	float radius;
};

layout (set = 0, binding = 0) uniform UBO 
{
	vec4 viewPos;
	Light lights[256];
	int lightCount;
	float ambient;
	float tone;
	float gamma;
} ubo;

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

void main() 
{
	// Get G-Buffer values
	vec3 fragPos = texture(position_tex, inUV).rgb;
	float mask = texture(lightning_mask_tex, inUV).r;
	vec3 normal = texture(normal_tex, inUV).rgb;
	vec4 albedo = texture(albedo_tex, inUV);

	//outFragcolor = vec4(vec3(mask), 1.0);
	//return;
	
	// Ambient part
	vec3 fragcolor  = albedo.rgb * pow(ubo.ambient, mask);
	
	for(int i = 0; i < ubo.lightCount * mask; ++i)
	{
		// Vector to light
		vec3 L = ubo.lights[i].position.xyz - fragPos;
		// Distance from light to fragment position
		float dist = length(L);

		// Viewer to fragment
		vec3 V = ubo.viewPos.xyz - fragPos;
		V = normalize(V);
		
		//if(dist < ubo.lights[i].radius)
		{
			// Light to fragment
			L = normalize(L);

			// Attenuation
			float atten = ubo.lights[i].radius / (pow(dist, 2.0) + 1.0);

			// Diffuse part
			vec3 N = normalize(normal);
			float NdotL = max(0.0, dot(N, L));
			vec3 diff = ubo.lights[i].color * albedo.rgb * NdotL * atten;

			// Specular part
			// Specular map values are stored in alpha of albedo mrt
			vec3 R = reflect(-L, N);
			float NdotR = max(0.0, dot(R, V));
			vec3 spec = ubo.lights[i].color * albedo.a * pow(NdotR, 16.0) * atten;

			fragcolor += diff + spec;	
		}	
	}

	// Tone mapping
	fragcolor = Uncharted2Tonemap(fragcolor * ubo.tone);
	fragcolor = fragcolor * (1.0 / Uncharted2Tonemap(vec3(11.2)));	
	// Gamma correction
	fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
   
  	outFragcolor = vec4(fragcolor, 1.0 );	
}