#version 330 core

out vec4 color;

in vec4 clipSpace;
in vec3 dirCam;

uniform float u_time;

uniform sampler2D reflectionShader;
uniform sampler2D refractionShader;
uniform sampler2D dudvMap;

void main(){
  vec2 ndc = (clipSpace.xy/clipSpace.w)/2+.5;
  
  vec2 refl = vec2(ndc.x, -ndc.y);
  vec2 refr = vec2(ndc.x, ndc.y);

  vec2 uvDistorsion = (clipSpace.xy/clipSpace.w)*2+2;
  float moveWater =  mod(0.01*u_time,1.)*5;
  vec2 distortion1 = (texture2D(dudvMap, vec2(uvDistorsion.x + moveWater, uvDistorsion.y) ).rg * 2.0 - 1.0)*0.005;
  vec2 distortion2 = (texture2D(dudvMap, vec2(uvDistorsion.x + moveWater, uvDistorsion.y + moveWater ) ).rg * 2.0 - 1.0)*0.005;
  vec2 distortion = distortion1 + distortion2;

  // clamp is to avoid shift "glitching"
  refr+=distortion;
  refr = clamp(refr, 0.001, 0.999); 

  refl+=distortion;
  refl.x = clamp(refl.x, 0.001, 0.999);
  refl.y = clamp(refl.y, -0.999, -0.001);

  vec4 reflTex =  texture2D(reflectionShader, refl);
  vec4 refrTex =  texture2D(refractionShader, refr);
  
  float transparency = normalize(dirCam).y; // dot product with (0,1,0)
  transparency = pow(transparency, .7);

  color = mix(reflTex, refrTex, transparency);
  color = mix(color, vec4(0.3216, 0.6706, 1.0, 1.0), 0.2);
}



