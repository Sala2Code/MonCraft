#version 330 core

out vec4 color;

in vec2 fragCoord; // [-1;1] X [-1;1]


uniform sampler2D texFlare;
uniform float brightness;

void main(){
    vec2 uv = fragCoord + .5;
    uv = clamp(uv, 0., 1.);

    color = texture(texFlare, uv);
    color.a *=  brightness;
}