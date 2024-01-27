#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNorm;


out vec2 UV;

uniform mat4 MVP;

void main(){
	UV = aUV;
	gl_Position =  MVP * vec4(aPos,1);
}

