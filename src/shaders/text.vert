#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;


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
    vec4 pos = push.modelMatrix * vec4(inPosition, 0.0, 1.0);

    fragColor = inColor;     
    fragTexCoord = inTexCoord;

    gl_Position = pos;
}