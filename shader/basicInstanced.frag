#version 330 core

out vec4 color;

in vec2 UV;
in float ID;

uniform float shiftID;
uniform sampler2DArray arrTexture;


void main(){
  	vec4 MaterialDiffuseColor = texture(arrTexture, vec3(UV, ID+shiftID));
	color = MaterialDiffuseColor;	
}
