#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

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

} push;

void main() {
    gl_Position = global.projectionMatrix * global.viewMatrix * push.modelMatrix * vec4(inPosition.x, inPosition.y ,inPosition.z, 1.0);
    fragColor = inColor;
     
    fragNormal = vec3((push.normalMatrix * vec4(inNormal, 0)).xyz);
    fragTexCoord = inTexCoord;
}