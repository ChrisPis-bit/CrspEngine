#version 450

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


void main(){

}