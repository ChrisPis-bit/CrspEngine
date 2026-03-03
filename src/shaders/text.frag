#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 texCoord;
layout (location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 lightSpaceMat;
    vec3 lightDir;
} global;

//push constants block
layout( push_constant ) uniform constants
{
	vec2 position;
    vec2 scale;
    float rotation;
} pushConstants;


void main(){

	//outColor = vec4(1.0);
    float t = texture(texSampler, texCoord).r;
	outColor = vec4(fragColor * t, t);
    //outColor = vec4(1.0, 0.0,0.0,1.0);
}