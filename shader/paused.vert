#version 330 core

layout(location = 0) in vec3 position;

// out vec2 fragCoord;

void main(){
	// fragCoord = position.xy; // 2d
	gl_Position = vec4(position, 1.);
}
