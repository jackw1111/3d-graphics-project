/*
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 */

// Color computation inspired by: https://www.shadertoy.com/view/Ms2SD1

// I was looking for waves that would look good on a flat 2D plane.
// I didn't really find something to my liking so I tinkered a bit
// (with some inspiration from other shaders obviously).
// I thought the end result was decent so I wanted to share

const float gravity = 1.0;
const float waterTension = 0.01;
const vec3 skyCol1 = vec3(0.2, 0.4, 0.6);
const vec3 skyCol2 = vec3(0.4, 0.7, 1.0);
const vec3 sunCol  =  vec3(8.0,7.0,6.0)/8.0;
const vec3 seaCol1 = vec3(0.1,0.2,0.2);
const vec3 seaCol2 = vec3(0.8,0.9,0.6);

float gravityWave(in vec2 p, float k, float h) {
  float w = sqrt(gravity*k*tanh(k*h));
  return sin(p.y*k + w*iTime);
}

float capillaryWave(in vec2 p, float k, float h) {
  float w = sqrt((gravity*k + waterTension*k*k*k)*tanh(k*h));
  return sin(p.y*k + w*iTime);
}

void rot(inout vec2 p, float a) {
  float c = cos(a);
  float s = sin(a);
  p = vec2(p.x*c + p.y*s, -p.x*s + p.y*c);
}

float seaHeight(in vec2 p) {
  float height = 0.0;

  float k = 1.0;
  float kk = 1.3;
  float a = 0.25;
  float aa = 1.0/(kk*kk);

  float h = 10.0;
  p *= 0.5;

  for (int i = 0; i < 3; ++i) {
    height += a*gravityWave(p + float(i), k, h);
    rot(p, float(i));
    k *= kk;
    a *= aa;
  }
  
  for (int i = 3; i < 7; ++i) {
    height += a*capillaryWave(p + float(i), k, h);
    rot(p, float(i));
    k *= kk;
    a *= aa;
  }

  return height;
}

vec3 seaNormal(in vec2 p, in float d) {
  vec2 eps = vec2(0.001*pow(d, 1.5), 0.0);
  vec3 n = vec3(
    seaHeight(p + eps) - seaHeight(p - eps),
    2.0*eps.x,
    seaHeight(p + eps.yx) - seaHeight(p - eps.yx)
  );
  
  return normalize(n);
}

vec3 sunDirection() {
  vec3 dir = normalize(vec3(0, 0.15, 1));
  return dir;
}

vec3 skyColor(vec3 rd) {
  vec3 sunDir = sunDirection();

  float sunDot = max(dot(rd, sunDir), 0.0);
  
  vec3 final = vec3(0.0);

  final += mix(skyCol1, skyCol2, rd.y);

  final += 0.5*sunCol*pow(sunDot, 20.0);

  final += 4.0*sunCol*pow(sunDot, 400.0);
    
  return final;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
  vec2 q=fragCoord.xy/iResolution.xy; 
  vec2 p = -1.0 + 2.0*q;
  p.x *= iResolution.x/iResolution.y;

  vec3 ro = vec3(0.0, 10.0, 0.0);
  vec3 ww = normalize(vec3(0.0, -0.1, 1.0));
  vec3 uu = normalize(cross( vec3(0.0,1.0,0.0), ww));
  vec3 vv = normalize(cross(ww,uu));
  vec3 rd = normalize(p.x*uu + p.y*vv + 2.5*ww);

  vec3 col = vec3(0.0);

  float dsea = (0.0 - ro.y)/rd.y;
  
  vec3 sunDir = sunDirection();
  
  vec3 sky = skyColor(rd);
    
  if (dsea > 0.0) {
    vec3 p = ro + dsea*rd;
    float h = seaHeight(p.xz);
    vec3 nor = mix(seaNormal(p.xz, dsea), vec3(0.0, 1.0, 0.0), smoothstep(0.0, 200.0, dsea));
    float fre = clamp(1.0 - dot(-nor,rd), 0.0, 1.0);
    fre = pow(fre, 3.0);
    float dif = mix(0.25, 1.0, max(dot(nor,sunDir), 0.0));
    
    vec3 refl = skyColor(reflect(rd, nor));
    vec3 refr = seaCol1 + dif*seaCol2*0.1; 
    
    col = mix(refr, 0.9*refl, fre);
      
    float atten = max(1.0 - dot(dsea,dsea) * 0.001, 0.0);
    col += seaCol2 * (p.y - h) * 2.0 * atten;
      
    col = mix(col, sky, 1.0 - exp(-0.01*dsea));
  } else {
    col = sky;
  }

  fragColor = vec4(col,1.0);
}