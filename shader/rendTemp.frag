#version 330 core

out vec4 fragcolor;

uniform vec2 u_resolution;
uniform sampler2D screenTexture;

void main(){
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    vec3 color = texture2D(screenTexture, st).rgb;
    fragcolor = vec4(color, 1.);
}