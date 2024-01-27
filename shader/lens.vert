#version 330 core

layout(location = 0) in vec3 position;

out vec2 fragCoord;

uniform float zLens;
uniform vec2 flarePos;
uniform vec2 scale;


void main(){
	fragCoord = position.xy; // 2d

	vec2 screenPos = fragCoord * scale + flarePos;
	screenPos.x = screenPos.x * 2.0 - 1.0;
	screenPos.y = screenPos.y * -2.0 + 1.0;

	gl_ClipDistance[0] = 0;

	gl_Position = vec4(screenPos, 0.02-0.01*zLens, 1.); // TODO : limit of z-depth	 
}
