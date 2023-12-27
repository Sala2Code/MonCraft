#version 330 core

layout(location = 0) out vec4 fragColor;

in vec2 fragCoord; // [-1;1] X [-1;1]

uniform vec2 u_resolution; // largeur, hauteur
uniform vec2 angleView; // spherique, phi, theta
uniform float u_time;

const float PI = 3.14159265;
const float VIEWER_FOV = 0.68;
const mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 ); // to rotate the uv coordinates and get different noise patterns

const vec3  DAYSKY_COLOUR   = vec3( 0.37, 0.54, 0.97 );
const vec3  NIGHTSKY_COLOUR = vec3(0.02, 0.03, 0.11);
const vec3  SUNSET_COLOUR   = vec3( 1.00, 0.32, 0.01 );

float TIME_WORLD = u_time * 0.02 ;
float angleSun  = TIME_WORLD * PI * 2.;

float dayPhase = cos( fract(TIME_WORLD) * PI * 2.) * 0.5 + 0.5;
float sunsetPhase = cos( fract(TIME_WORLD) * PI * 4. + PI ) * 0.5 + 0.5;

vec3 sunDir = normalize( vec3( sin( angleSun ), cos( angleSun), 0.) ); // zenith ; z to incline 
vec3 moonDir = normalize( vec3( sin( angleSun + PI), cos( angleSun + PI), 0.) ); // zenith ; z to incline 


mat2 rotate_mat2( float a ) {    
   float c = cos( a );
   float s = sin( a );
   return mat2( c, s, -s, c );
}

// ? problem to implement texture on moon so I use procedural texture : perlin noise ; it's good too (and useful for clouds !)
vec3 random3(vec3 coord) {   
    float rand = 4096.0*sin(dot(coord,vec3(17, 59, 15))); // pseudo random
    vec3 r;
    // Multiply each time by a factor to not make "pattern" ; Remove them and look 
    r.z = fract(rand);
    rand *= .175;
    r.x = fract(rand);
    rand *= .891;
    r.y = fract(rand);
    return 4*(r-.5); // [0;1] -> [-0.5*blur;0.5*blur]
}
float perlin3D(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);

    // interpolation ; x-> 6x^5 − 15x^4 + 10x^3 works too
    vec3 u = f*f*(3.0-2.0*f);
    // interpolation between 8 corners
    return mix( 
            mix( // z = 0
                mix( // y = 0
                    dot( random3(i + vec3(0.0,0.0,0.0) ), f - vec3(0.0,0.0,0.0) ),
                    dot( random3(i + vec3(1.0,0.0,0.0) ), f - vec3(1.0,0.0,0.0) ), 
                    u.x),
                mix( // y = 1
                    dot( random3(i + vec3(0.0,1.0,0.0) ), f - vec3(0.0,1.0,0.0) ),
                    dot( random3(i + vec3(1.0,1.0,0.0) ), f - vec3(1.0,1.0,.0) ), 
                    u.x), 
                u.y),
                
            mix( // z = 1
                mix( // y = 0
                    dot( random3(i + vec3(0.0,0.0,1.0) ), f - vec3(0.0,0.0,1.0) ),
                    dot( random3(i + vec3(1.0,0.0,1.0) ), f - vec3(1.0,0.0,1.0) ), 
                    u.x),
                mix( // y= 1
                    dot( random3(i + vec3(0.0,1.0,1.0) ), f - vec3(0.0,1.0,1.0) ),
                    dot( random3(i + vec3(1.0,1.0,1.0) ), f - vec3(1.0,1.0,1.0) ), 
                    u.x), 
                u.y),

            u.z);
}


vec3 skyAmbient( vec3 dir){ // "hue" color according to the sun position (phase)
   float sun_azimuth  = dot( dir.xz, sunDir.xz ) * 0.4 + 0.9;
   vec3 ambient  = DAYSKY_COLOUR * (dayPhase-sunsetPhase*0.1) + 0.2*SUNSET_COLOUR * (1.0-dir.y) * sunsetPhase * sun_azimuth + NIGHTSKY_COLOUR * (1.0-dayPhase);
   
   return ambient;
}
vec3 sunAmbient(vec3 rayDir){
   float sunTheta  = dot( rayDir, sunDir );
   float sunSize   = max( 0.0, sunTheta * 0.5 + 0.5); 
   float sunShape  = pow( sunSize, 200.0 ); // blur 

   vec3 sunColour  = mix( vec3(1.0), SUNSET_COLOUR, sunsetPhase*0.1 );
   sunColour *= sunShape;
   return sunColour;

}
// if perlin noise is ugly, to replace per a texture :take orthogonal plane to the rayDir and you get uv coords
vec3 moonAmbient(vec3 rayDir){
   float moonTheta  = dot( rayDir, moonDir );
   float moonSize   = max( 0.0, moonTheta * 0.5 + 0.5 ); 
   float moonShape  = pow( moonSize, 200.0 ); // blur 

   vec3 moonColour  = vec3(1.);
   moonColour *= moonShape;

   vec3 norm = rayDir - moonDir;
   float n = (perlin3D(norm*10.)*0.5+0.5); 
   n= clamp (n, 0.2, .8);

   vec3 moonMask = step(0.5, moonColour);

   vec3 moonLight = moonColour;
   moonLight = (1.-moonMask)*moonColour;;
   return moonMask * n * moonColour + moonLight;
}

float clouds(vec2 st ) {
   float time = TIME_WORLD;
	float f = 0.0;
   st += time;
   float weight = 0.8;
   for (int i=0; i<10; i++){ // complexity of shape : iterations > 7
		f += weight*perlin3D( vec3(st,time) );
      st = m*st + time;
		weight *= 0.6;
   }
   return 6.*f*f;

}



vec3 cloudsColours(vec3 rayDir){
   float cloud_accum = 0.0;
   
   vec2  cloud_plane    = (rayDir.xz / rayDir.y) * 256.;
   if(length(cloud_plane) < 1000.0) {
      vec2  tex_uv = cloud_plane * vec2( 0.004, 0.004 ) + vec2( 0.0,TIME_WORLD*0.06 ); // density + speed
      cloud_accum += pow( clouds(tex_uv), 10.0 );

      cloud_accum = clamp(cloud_accum, 0.0, .7);
      // vec3 cloud_colour = mix(SUNSET_COLOUR,vec3(1.0),dayPhase);
      return vec3(cloud_accum); // to avoid .../0
   }
   return vec3(0.0);
}

vec3 sky( vec3 rayDir, bool isCloud){
   vec3 skyColour  = skyAmbient( rayDir);
   vec3 sunColour = sunAmbient(rayDir);
   vec3 moonColour = moonAmbient(rayDir);
         
   vec3 comp = skyColour + sunColour + moonColour;
   if(isCloud){
      vec3 cloudsColour = cloudsColours(rayDir);
      return comp + cloudsColour;
   }
   return comp;
}

vec3 rayDir_spherical( vec2 uv, float pitch, float yaw, vec2 ratio ){
   vec3 rayDir  = normalize( vec3( (uv-0.5) * ratio, VIEWER_FOV) );
   rayDir.yz   *= rotate_mat2( pitch );
   rayDir.xz   *= rotate_mat2( yaw );
   return rayDir;
}

void main(){
   vec2 uv = (fragCoord + 1.)*.5; // [0;1] X [0;1]
   vec2 ratio = vec2( u_resolution.x/u_resolution.y, 1.0 );
   
   float angleYaw   = -angleView.x; // phi
   float anglePitch = -angleView.y; // theta
   
   vec3 rayDir = rayDir_spherical( uv, anglePitch, angleYaw, ratio );
   vec3 color = sky( normalize(rayDir),(rayDir.y>0) ? true : false);

   fragColor = vec4(color, 1.);


}