#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 0) out vec4 outColor;

layout(binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
} global;

//push constants block
layout( push_constant ) uniform constants
{
    mat4 normalMatrix;
	mat4 modelMatrix;
} pushConstants;

void main(){
	outColor = vec4(fragColor, 1.0);
}