#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;

out vec2 UV;
out float iBiome;

uniform mat4 MVP;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 LightPosition_worldspace;

void main(){
    UV = vertexUV;
    iBiome = -1; // No biome for those using this vertex

// ! PAS LIEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE

    gl_ClipDistance[0] = 0;
    gl_Position =  MVP * vec4(vertexPosition,1);
}
