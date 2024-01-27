#version 330 core

layout(location = 0) in vec3 position;

out vec2 fragCoord;

void main(){
	fragCoord = position.xy; // 2d
	gl_ClipDistance[0] = 0;

	gl_Position = vec4(fragCoord,0.999, 1.);
	 
}

