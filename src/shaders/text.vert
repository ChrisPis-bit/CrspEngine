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
    bool renderShadows;
} global;

//push constants block
layout( push_constant ) uniform constants
{
	vec2 position;
    vec2 scale;
    float rotation;
} push;

void main() {
    vec2 pos = inPosition * push.scale + push.position;

    fragColor = inColor;     
    fragTexCoord = inTexCoord;

    gl_Position = vec4(pos, 1.0, 1.0);
}