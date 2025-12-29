#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform lightUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 lightDir;
} light;

//push constants block
layout( push_constant ) uniform constants
{
	mat4 modelMatrix;
} push;

void main() {
    gl_Position = light.projectionMatrix * light.viewMatrix * push.modelMatrix * vec4(inPosition.x, inPosition.y ,inPosition.z, 1.0);
}