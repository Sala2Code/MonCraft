#version 330 core
out vec4 color;

in vec2 UV;
in vec3 posFrag;
// in vec3 Position_worldspace;
// in vec3 Normal_cameraspace;
// in vec3 EyeDirection_cameraspace;
// in vec3 LightDirection_cameraspace;
in float iBiome;


uniform float u_time;
uniform sampler2D myTextureSampler;
// uniform vec3 LightPosition_worldspace;

const float PI = 3.1415;

float TIME_WORLD = u_time * 0.02 ;
float angleSun  = TIME_WORLD * PI * 2.;

float sunsetPhase = cos( fract(TIME_WORLD) * PI * 4. + PI ) * 0.5 + 0.5;
vec3 sunDir = normalize( vec3( sin( angleSun ), cos( angleSun ), 0.) ); // zenith ; z to incline 
vec3 moonDir = normalize( vec3( sin( angleSun + PI), cos( angleSun + PI), 0.) ); // zenith ; z to incline 
vec3 norm = vec3(0,1,0);
const vec3  SUNSET_COLOUR   = vec3( 1.00, 0.32, 0.01 );
vec3 lightColor =  mix( vec3(1.0), SUNSET_COLOUR, sunsetPhase*0.2 );


void main(){
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
  vec3 MaterialAmbientColor = vec3(0.6,0.6,0.6) * MaterialDiffuseColor;

  vec3 lightSun = normalize(sunDir*10. - posFrag);
  float diffSun = max(dot(norm, lightSun), 0.0);
  vec3 diffuseSun = diffSun * lightColor;

  vec3 lightMoon = normalize(moonDir*10. - posFrag);
  float diffMoon = max(dot(norm, lightMoon), 0.0);
  vec3 diffuseMoon = diffMoon * lightColor;

  color.rgb =  (MaterialAmbientColor + 0.9*diffuseSun + 0.3*diffuseMoon) * MaterialDiffuseColor;
  color.a = 1;


  // Biome
  // if(iBiome==0.0){
  //   color.r=0.;
  // }else if(iBiome==1.0){
  //   color.g=0;
  // }else if(iBiome==2.0){
  //   color.b=0.;
  // }

}