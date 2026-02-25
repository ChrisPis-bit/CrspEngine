#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 lightSpaceCoord;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D shadowTexSampler;
layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 lightSpaceMat;
    vec3 lightDir;
} global;

layout(set = 1, binding = 0) uniform localUBO {
    vec4 color;
} local;

//push constants block
layout( push_constant ) uniform constants
{
    mat4 normalMatrix;
	mat4 modelMatrix;
} pushConstants;

const float AMBIENT = 0.02;

float shadowCalculation(vec4 lightSpaceCoord, vec3 lightDir, vec3 normal){
    vec3 projCoords = lightSpaceCoord.xyz / lightSpaceCoord.w;

    float currentDepth = projCoords.z;

    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowTexSampler, projCoords.xy).r;

    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);  
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main(){
    vec3 normal = normalize(fragNormal);
    float diffuse = dot(normal, global.lightDir);
    float shadow = shadowCalculation(lightSpaceCoord, global.lightDir, normal);

    float lightIntensity = AMBIENT + (1.0 - shadow) * max(diffuse, 0);

	outColor = texture(texSampler, texCoord) * lightIntensity * local.color;

}