#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;

out vec2 UV;
out vec3 posFrag;
out float iBiome;

uniform mat4 MVP;


void main(){
    UV = vertexUV;
    posFrag = vertexPosition;
    iBiome = -1; // No biome for those using this vertex

    gl_ClipDistance[0] = 0;


    gl_Position =  MVP * vec4(vertexPosition,1);
}