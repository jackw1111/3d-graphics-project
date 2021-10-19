#version 330 core
in vec2 TexCoords;
uniform vec2 iMouse;
uniform vec2 iResolution;
uniform float iTime;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;
#define PI 3.14159265359
#define TWOPI 6.28318530718
// "RayMarching starting point" 
// by Martijn Steinrucken aka The Art of Code/BigWings - 2020
// The MIT License
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// Email: countfrolic@gmail.com
// Twitter: @The_ArtOfCode
// YouTube: youtube.com/TheArtOfCodeIsCool
// Facebook: https://www.facebook.com/groups/theartofcode/
//
// You can use this shader as a template for ray marching shaders

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .001

#define S smoothstep
#define T iTime

const int MAT_BASE = 1;
const int MAT_BARS = 2;
const int MAT_BALLS = 3;
const int MAT_LINE = 4;

mat2 Rot(float a) {
    float s=sin(a), c=cos(a);
    return mat2(c, -s, s, c);
}

float sdBox(vec3 p, vec3 s) {
    p = abs(p)-s;
    return length(max(p, 0.))+min(max(p.x, max(p.y, p.z)), 0.);
}


float sdBox(vec2 p, vec2 s) {
    p = abs(p)-s;
    return length(max(p, 0.))+min(max(p.x, p.y), 0.);
}

float sdLineSeg(vec3 p, vec3 a, vec3 b) {
    vec3 ap = p-a;
    vec3 ab = b-a;
    float t = clamp(dot(ap, ab) / dot(ab, ab),0,1);
    vec3 c = a + ab*t;
    return length(p-c);   
}

vec2 sdBall(vec3 p, float a) {
    p.y -= 1.01;
    p.xy *= Rot(a);
    p.y += 1.01;
    float ball = length(p) - 0.15;
    float ring = length(vec2(length(p.xy - vec2(0, 0.15)) - 0.03, p.z)) -0.01;
    ball = min(ball, ring);
    p.z = abs(p.z);
    float line = sdLineSeg(p, vec3(0, 0.15, 0), vec3(0,1.01, 0.4)) - 0.005;
    float d = min(ball, line);
    return vec2(d, d==ball ? MAT_BALLS : MAT_LINE);
}

float GetDist(vec3 p) {
    float base = sdBox(p, vec3(1.0, 0.1, 0.5)) - .1;
    float bar = length(vec2(sdBox(p.xy, vec2(0.8,1.4))-0.15, abs(p.z) - 0.4)) -.04;
    float a = sin(iTime);
    float a1 = min(a, 0);
    float a5 = max(a,0);
    float b1 = sdBall(p-vec3(0.6,0.5,0), a1).x;
    float b2 = sdBall(p-vec3(0.3,0.5,0), (a+a1)*0.05).x;
    float b3 = sdBall(p-vec3(0,0.5,0), a*0.05).x;
    float b4 = sdBall(p-vec3(-0.3,0.5,0), (a+a5)*0.05).x;
    float b5 = sdBall(p-vec3(-0.6,0.5,0), a5).x;
    float balls = min(b1, min(b2, min(b3, min(b4, b5))));
    float d = min(base, bar);
    d = min(d, balls);
    d = max(d, -p.y);

    return d;
}

vec2 Min(vec2 a, vec2 b) {
    return a.x<b.x ? a : b;
}

int GetMat(vec3 p) {
    float base = sdBox(p, vec3(1.0, 0.1, 0.5)) - .1;
    float bars = length(vec2(sdBox(p.xy, vec2(0.8,1.4))-0.15, abs(p.z) - 0.4)) -.04;
    float a = sin(iTime);
    float a1 = min(a, 0);
    float a5 = max(a,0);
    vec2 b1 = sdBall(p-vec3(0.6,0.5,0), a1);
    vec2 b2 = sdBall(p-vec3(0.3,0.5,0), (a+a1)*0.05);
    vec2 b3 = sdBall(p-vec3(0,0.5,0), a*0.05);
    vec2 b4 = sdBall(p-vec3(-0.3,0.5,0), (a+a5)*0.05);
    vec2 b5 = sdBall(p-vec3(-0.6,0.5,0), a5);
    vec2 balls = Min(b1, Min(b2, Min(b3, Min(b4, b5))));
    float d = min(base, bars);
    d = min(d, balls.x);
    base = max(base, -p.y);
    d = max(d, -p.y);

    int mat = 0;

    if (d==base) {
        mat = MAT_BASE;
    } else if (d==bars){
        mat = MAT_BARS;
    } else if (d==balls.x){
        mat = int(balls.y);
    }

    return mat;
}


float RayMarch(vec3 ro, vec3 rd) {
    float dO=0.;
    
    for(int i=0; i<MAX_STEPS; i++) {
        vec3 p = ro + rd*dO;
        float dS = GetDist(p);
        dO += dS;
        if(dO>MAX_DIST || abs(dS)<SURF_DIST) break;
    }
    
    return dO;
}

vec3 GetNormal(vec3 p) {
    float d = GetDist(p);
    vec2 e = vec2(.001, 0);
    
    vec3 n = d - vec3(
        GetDist(p-e.xyy),
        GetDist(p-e.yxy),
        GetDist(p-e.yyx));
    
    return normalize(n);
}

vec3 GetRayDir(vec2 uv, vec3 p, vec3 l, float z) {
    vec3 f = normalize(l-p),
        r = normalize(cross(vec3(0,1,0), f)),
        u = cross(f,r),
        c = f*z,
        i = c + uv.x*r + uv.y*u,
        d = normalize(i);
    return d;
}

void main(){
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    vec2 m = iMouse.xy/iResolution.xy;

    vec3 ro = vec3(0, 3, -3);
    ro.yz *= Rot(-m.y*3.14+1.);
    ro.xz *= Rot(-m.x*6.2831);
    
    vec3 rd = GetRayDir(uv, ro, vec3(0,0.75,0), 2.);
    vec3 col = vec3(0);
   
    float d = RayMarch(ro, rd);

    if(d<MAX_DIST) {
        vec3 p = ro + rd * d;
        vec3 n = GetNormal(p);
        vec3 r = reflect(rd, n);

        float dif = dot(n, normalize(vec3(1,2,3)))*.5+.5;
        col = vec3(dif);

        int mat = GetMat(p);
        if (mat == MAT_BASE) {
            col *= 0.1;
        } else if (mat == MAT_BARS) {
            col *= vec3(0,0.0,1.0);
        } else if (mat == MAT_BALLS) {
            col *= vec3(1.0,0,0.0);
        } else if (mat == MAT_LINE) {
            col *= vec3(0,1,0);
        }
    }
    
    col = pow(col, vec3(.4545));    // gamma correction
    
    fragColor = vec4(col,1.0);
}
