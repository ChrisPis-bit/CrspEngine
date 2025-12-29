#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D shadowTexSampler;

layout(set = 0, binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 lightDir;
} global;

//push constants block
layout( push_constant ) uniform constants
{
    mat4 normalMatrix;
	mat4 modelMatrix;
} pushConstants;

const float AMBIENT = 0.02;

void main(){
    float diffuse = dot(normalize(normal), global.lightDir);
    float lightIntensity = AMBIENT + max(diffuse, 0);

	outColor = texture(texSampler, texCoord) * lightIntensity;
}