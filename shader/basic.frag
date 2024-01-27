#version 330 core

out vec4 color;

in vec2 UV;

uniform sampler2D texture;

void main(){
	vec4 MaterialDiffuseColor = texture2D(texture, UV);
	color = MaterialDiffuseColor;
}
