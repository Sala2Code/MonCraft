#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;


void main(){
    vec3 color = texture2D(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.);
    // fragcolor = vec4(vec3(1.), 1.);
}