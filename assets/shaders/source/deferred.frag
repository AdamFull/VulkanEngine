#version 450

layout (set = 1, binding = 0) uniform sampler2D position_tex;
layout (set = 1, binding = 1) uniform sampler2D normal_tex;
layout (set = 1, binding = 2) uniform sampler2D albedo_tex;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragcolor;

struct Light {
	vec4 position;
	vec3 color;
	float radius;
};

layout (set = 0, binding = 0) uniform UBO 
{
	Light lights[6];
	vec4 viewPos;
} ubo;

void main() 
{
	// Get G-Buffer values
	vec3 fragPos = texture(position_tex, inUV).rgb;
	vec3 normal = texture(normal_tex, inUV).rgb;
	vec4 albedo = texture(albedo_tex, inUV);

	// Render-target composition

	#define lightCount 6
	#define ambient 0.0
	
	// Ambient part
	vec3 fragcolor  = albedo.rgb * ambient;
	
	for(int i = 0; i < lightCount; ++i)
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
   
  outFragcolor = vec4(fragcolor, 1.0);	
}