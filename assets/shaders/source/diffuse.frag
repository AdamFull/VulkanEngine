#version 450

//Default
layout(set = 1, binding = 0) uniform sampler2D brdflut_tex;
layout(set = 1, binding = 1) uniform samplerCube irradiate_cube_tex;
layout(set = 1, binding = 2) uniform samplerCube prefiltred_cube_tex;

//Additional
layout(set = 1, binding = 3) uniform sampler2D color_tex;
layout(set = 1, binding = 4) uniform sampler2D metalRough_tex;
layout(set = 1, binding = 5) uniform sampler2D specGloss_tex;
layout(set = 1, binding = 6) uniform sampler2D emissive_tex;
layout(set = 1, binding = 7) uniform sampler2D normal_tex;
layout(set = 1, binding = 8) uniform sampler2D height_tex;
layout(set = 1, binding = 9) uniform sampler2D ao_tex;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in vec3 lightColor;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec3 viewPos;
layout(location = 5) in vec3 lightPos;

layout(location = 0) out vec4 outColor;

#define lightRadius 30.0
const float ambient = 0.1;
const float gamma = 2.2;

//From https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float height_scale, float minLayers, float maxLayers)
{ 
    vec2  currentTexCoords = texCoords;
    float currentDepthMapValue = texture(height_tex, fragTexCoord).r;

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
        currentDepthMapValue = texture(height_tex, fragTexCoord).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(height_tex, fragTexCoord).r - currentLayerDepth + layerDepth;
    
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;    
}



void main() 
{
	float invRadius = 1.0/lightRadius;
    vec3 normal = (texture(normal_tex, fragTexCoord).rgb * 2.0 - 1.0);

    float ao_factor = texture(ao_tex, fragTexCoord).r;
    vec4 emission = texture(emissive_tex, fragTexCoord);

	vec4 color = texture(color_tex, fragTexCoord);
    
	vec3 lightDir = normalize(lightPos - fragPos);
    float diffuse = max(dot(lightDir, normal), 0.15);

    float dist = length(lightDir);
    float atten = max(clamp(1.0 - invRadius * sqrt(dist), 0.0, 1.0), ambient);

	vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float specular = pow(max(dot(normal, halfwayDir), 0.0), 4.0);

	vec4 specularColor = texture(specGloss_tex, fragTexCoord);

	outColor = (color * atten + (diffuse * ao_factor * color + 0.5 * specular * specularColor + emission)) * atten;
}
