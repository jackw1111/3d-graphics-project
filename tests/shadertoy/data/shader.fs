#version 330 core

in vec2 TexCoord;
uniform vec2 iMouse;
uniform vec2 iResolution;
uniform float iTime;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
out vec4 fragColor;
vec4 fragCoord = gl_FragCoord;

#define PI 3.14159265359
#define TWOPI 6.28318530718


#define A 3.545
#define B 1.15
#define M smoothstep

// rotate on axis Z
vec3 Z(vec3 v, float a)
{
    vec3 vo = v; float c = cos(a); float s = sin(a);
    v.x = c * vo.x - s * vo.y;
    v.y = s * vo.x + c * vo.y;
    return v;
}

// Rotate on axis Y
vec3 Y(vec3 v, float a)
{
    vec3 vo = v; float c = cos(a); float s = sin(a);
    v.x = c * vo.x - s * vo.z;
    v.z = s * vo.x + c * vo.z;
    return v;
}

// Torus82
float T(vec3 p, vec2 t)
{
    vec2 q = vec2(sqrt(p.x * p.x + p.y * p.y) - t.x, p.z);
    q = q*q; q = q*q; q = q*q;
    return pow(q.x + q.y, 1. / 8.) - t.y;
}

// Gear
float G(vec3 p, float a, float r)
{
    float d = 1.;
    for (int i = 0; i < 6; i++)
    {
        vec3 q = abs(Z(p, a) - vec3(.0, r + .25, .0));
        d = min(d, max(q.z - .3, max((q.x * .6 + q.y * .7), q.y) - .25));
        a += 1.03;
    }
    
    return max(-d, T(p, vec2(r, .1)));
}

// sdBox
float X(vec3 p, vec3 b)
{
    vec3 d = abs(p) - b;
    return min(max(d.x, max(d.y, d.z)), .0) + length(max(d, .0));
}

// sdHexPrism
float H(vec3 p, vec2 h)
{
    vec3 q = abs(p);
    return max(q.z - h.y, max((q.x + q.y * .3), q.y) - h.x);
}

// smin
float S(float a, float b, float k)
{
    float h = clamp(.5 + .5 * (b - a) / k, 0., 1.);
    return mix(b, a, h) - k * h * (1. - h);
}

// Beenox
float BX(float t, vec3 p, float f)
{
    float s = t*t;
    if (t > A) s /= 2.;
    p = Y(p, s);
    
    float a = G(p, .6, f * .5);
    float b = length(p) - f * .33;
    return min(b, a);
}

// Black Ops 3
float BO(vec3 p, float x, float t)
{
    vec2 z = vec2(.12, .15);
    vec3 w = vec3(.12, z);
    
    // 1
    float a = X(p - vec3(x, .0, .0), vec3(.14, .6, .15));
    float b = X(p - vec3(x + .1, .48, .0), w);
    float c = H(Z(p - vec3(x + .065, .32,  .0), .8), z);
    float d = X(p - vec3(x + .1, -.49, .0), vec3(.12, .11, .15));
    float e = H(Z(p - vec3(x + .065, -.335, .0), -.75), z);
    a = min(a, min(e, min(d, min(c, b))));

    // 2
    c = mix(.0, .15, M(0., 4.5, t));
    d = mix(.0, .2, M(.7, .8, t));
    b = X(p - vec3(.0, .0, .0), vec3(d, .6, c));

    // 3
    c = X(p - vec3(-x, .0, .0), vec3(.13, .6, .15));
    d = X(p - vec3(-x - .1, .48, .0), w);
    e = H(Z(p - vec3(-x - .065, .32, .0), -.8), z);
    float f = X(p - vec3(-x - .1,-.48, .0), vec3(.12, .12, .15));
    float g = H(Z(p - vec3(-x - .065, -.335, .0), .75), z);
    c = min(c, min(g, min(f, min(e, d))));
    
    return min(c, min(b, a));
}

float map(float t, vec3 p)
{
    float x = 0.;
    float y = 0.;
	float d = 0.;
    if (t < A + B)
	{
        d = BX(t, p, 1.);
	}
    else if (t < 5.)
	{
        d = BX(A + B, p, 1.);
	}
    else if (t < 7.)
    {
        x = BX(A + B, p, M(1.1, 0., t - 5.));
        y = X(p, vec3(.15, .6 * M(0., 1., t - 5. + .4), .15));
        d = S(y, x, .1);
    }
    else
    {
        t -= 7.;
        x = .2 * pow(M(.0, 1., t), .5);
        y = .25 * pow(M(1.2, 3., t), .5);
        d = BO(p, x + y, t);
    }
	return d;
}

// plane - intersection with a predefined plane
vec3 P(vec3 p, vec3 d) 
{
    vec3 n = vec3(0., 1., 0.);
    float f = dot(-n * .8 - p, n) / dot(n, d);
    return p + d * f;
}

// Material used for the infinite plane
vec2 MAT(vec2 uv)
{
    vec2 uv2 = mod(uv, vec2(2.)) - mod(uv, vec2(1.));
    float d = uv2.x + uv2.y; 
    d = pow(d - 1., 2.) * .4;
    
    float s = d;
    d += s * .2;

    //d - diffuse, s - specular
    return vec2(d, s * s * .5 + .1);
}

// render background layer used for reflection
vec3 BG(vec3 p, vec3 d, vec3 l)
{
    // plane normal
    vec3 n = vec3(0., 1., 0.);
    
    // diffuse lighting for the plane
    float df = dot(n, l) * .5 + .5; 
    
    // to blend the plane with the sky
    float a = max(0., dot(d, -n)); // alpha - this coefficient is used
    
    // get the floor material
    vec2 m = MAT(P(p, d).xz); // x = diffuse coefficient, y = specular coefficient
    
    // atmosphere
    vec3 at = vec3(.3, .4, .7) * (1. - abs(d.y)) * 1.5;

    // calculate the planes color
    vec3 c = m.x * vec3(.4) * df + (m.y * .7) * (at * .2);
    
    // reduce fog
    a = pow(a, .4);
    
    // mix the plane with the sky
    return mix(at, c, a);
}

// Normal
vec3 N(float t, vec3 p, float e)
{
	float d = map(t, p);
	return normalize(
        vec3(
            map(t, p + vec3(e, 0, 0)) - d,
            map(t, p + vec3(0, e, 0)) - d,
            map(t, p + vec3(0, 0, e)) - d
        )
    );
}

void main()
{
    float t = 0.;
    if (iResolution.x <= 400.0) t = mod(iTime - 6.455, 15.);
    else t = mod(iTime, 15.);
    vec2 uv = fragCoord.xy / iResolution.xy - .5;
    uv.x *= iResolution.x / iResolution.y; //fix aspect ratio
    if (iResolution.y < 200.0) t = A;
    
    // ray position
    vec3 p = vec3(0., 0., 2.1); 
    
    // ray direction
    vec3 d = normalize(Y(vec3(uv, 1.), 3.14159));
    
    // light
    vec3 l = vec3(-.5, 1.2, .5);
    
    // background
    //vec3 c = vec3(0.);
    vec3 c = BG(p, d, l);

    // raymarching
    float di = 0.;
    for (int i = 0; i < 75; i++)
    {
        // scene
        di = map(t, p);
        p += d * di * .4;

        // too far away from the object or close enough, stop
        if (di > 5. || di < .03) 
            break;
    }

    if (di < .03)
    {
        vec3 n = N(t, p, .002);

        // a bit more wierd diffuse lighting, but looks great
        float diffuse = dot(l, n) * .1 + .5;
        diffuse = pow(diffuse, 1.5);

        // object color
        c = mix(vec3(.4, .4, 1.4), vec3(.1), M(B, 7., t));

        // reflection
        c = mix(c * diffuse, BG(p, reflect(d, n), l), (1.0 + dot(d, n)) * .6 + .2);

        // self occlusion
        c *= map(t, p + n) * .5 + .5;
    }    
    
    // fade in
    c = mix(c, vec3(0.), M(3., 0., t));

    // fade out
    c = mix(c, vec3(0.), M(14., 15., t)); 
    
    fragColor = vec4(c, 1.);
}
