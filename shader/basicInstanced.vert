#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;

layout(location = 3) in float iBiome_instance;
layout(location = 4) in mat4 MVP_instance;
layout(location = 8) in mat4 Models_instance;


out vec3 posFrag;
out vec2 UV;
out float iBiome;
// out vec3 Position_worldspace;
// out vec3 Normal_cameraspace;
// out vec3 EyeDirection_cameraspace;
// out vec3 LightDirection_cameraspace;

uniform vec3 LightPosition_worldspace;
uniform float up; // Above (1) / behind (-1)

const float WATER_HEIGHT = 30*0.1; // ! A mieux faire
vec4 plane = vec4(0,up,0,-WATER_HEIGHT * up);

void main(){
    posFrag = vertexPosition;
    UV = vertexUV;
    iBiome = iBiome_instance;

    gl_ClipDistance[0] = dot(Models_instance * vec4(vertexPosition,1), plane);
    // gl_ClipDistance[0] = 0;

    

    gl_Position =  MVP_instance * vec4(vertexPosition,1);
}