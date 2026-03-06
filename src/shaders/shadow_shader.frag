#version 450

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
	mat4 modelMatrix;
} push;


void main(){

}