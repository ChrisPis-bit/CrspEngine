#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform globalUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 lightSpaceMat;
    vec3 lightDir;
} global;

//push constants block
layout( push_constant ) uniform constants
{
	mat4 modelMatrix;
} push;

void main() {
    gl_Position = global.lightSpaceMat * push.modelMatrix * vec4(inPosition, 1.0);
}