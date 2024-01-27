#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;

layout(location = 3) in mat4 MVP_instance;
layout(location = 7) in mat4 Models_instance;

vec4 plane = vec4(0,.8,0,0);

void main(){
    vec4 worldPos = Models_instance * vec4(vertexPosition,1);
    gl_ClipDistance[0] = dot(worldPos, plane);

    gl_Position = MVP_instance * vec4(vertexPosition,1);
}