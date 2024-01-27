#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;

layout(location = 3) in mat4 MVP_instance;
layout(location = 7) in mat4 Models_instance;
layout(location = 11) in float idTex_instance;

out vec2 UV;
out float ID;

uniform float up; // Above (1) / behind (-1)
uniform float WATER_HEIGHT;

vec4 plane = vec4(0,up,0,-WATER_HEIGHT * up);

void main(){
    UV = vertexUV;
    ID = idTex_instance;

    gl_ClipDistance[0] = dot(Models_instance * vec4(vertexPosition,1), plane);

    gl_Position =  MVP_instance * vec4(vertexPosition,1);
}