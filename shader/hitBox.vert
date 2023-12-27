#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
// layout(location = 1) in vec2 vertexUV;
// layout(location = 2) in vec3 vertexNormals;


out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

uniform mat4 MVP;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 LightPosition_worldspace;

void main(){
    vec3 vertexPosition_cameraspace = ( View * Model * vec4(vertexPosition_modelspace,1)).xyz;

	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
	vec3 LightPosition_cameraspace = ( View * vec4(LightPosition_worldspace,1)).xyz;
    
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
}


