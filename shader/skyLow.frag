#version 330 core

layout(location = 0) out vec4 fragColor;

in vec2 fragCoord; // [-1;1] X [-1;1]

uniform float u_time;

const float PI = 3.14159265;

const vec3  DAYSKY_COLOUR   = vec3( 0.37, 0.54, 0.97 );
const vec3  NIGHTSKY_COLOUR = vec3(0.02, 0.03, 0.11);

float dayPhase = cos( fract(u_time) * PI * 2.) * 0.5 + 0.5;
float sunsetPhase = cos( fract(u_time) * PI * 4. + PI ) * 0.5 + 0.5;


void main(){
    vec3 color = DAYSKY_COLOUR * (dayPhase-sunsetPhase*0.1) + NIGHTSKY_COLOUR * (1.0-dayPhase);
    fragColor = vec4(color, 1.);
}