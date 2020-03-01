#ifndef VOLUMETRICMATERIAL_GLSL
#define VOLUMETRICMATERIAL_GLSL

// https://www.alanzucconi.com/2017/10/10/atmospheric-scattering-1/
// https://www.pbrt.org/fileformat-v3.html#media-world
// This material defineition follows the pbrt v3 heterogeneous participating media definition
struct Material
{
// The density grid
    sampler3D density;
// Absorption coefficient, default to (0.0011, 0.0024, 0.014, 1)
    vec4 sigma_a;
// Scattering coefficient, default to (2.55, 3.21, 3.77, 1)
    vec4 sigma_s;
// phase function assymetry factor, default to 0
    float g;
// Scale factor applied to the absorption and scattering coefficients
    float scale;
// step size for ray marching
    float stepsize;
// Transformlation matrix to the caconical [0, 1]^3 domain
    mat4 modelToDensity;
};

// Some usefull constant
float Inv4Pi = 0.07957747154594766788;

// The Henyey-Greenstein phase function
float phaseHG(float cosTheta, float g) {
    float denom = 1 + g * g + 2 * g * cosTheta;
    return Inv4Pi * (1 - g * g) / (denom * sqrt(denom));
}

/* -------------------------------------------------------------------------------------------------------------- */
/* The following two functions mut be adatped to the lighting process */
// compute the attenation of the light (miss the light intensity parameter ...
vec3 lightColor(Material volume, vec3 p, vec3 light) {
    // TBD
    return vec3(1);
}
// compute the scattering at p
vec3 inscatter(Material volume, vec3 p, vec3 dir, vec3 light) {
    return Inv4Pi*volume.sigma_s.rgb * phaseHG(dot(dir, light), volume.g) * lightColor(volume, p, light);
}

/* -------------------------------------------------------------------------------------------------------------- */

// march along the ray.
// Return in rgb the color acculumated into the volume and, in tr, the final transmittance
// p0 is the starting point on the boundary of the volume
// dir is the (normalized) direction of the ray
// light is the (normalized) direction toward the light source
// This functions assume that p0, dir and light are expressed in the unit cube frame [0,1]^3, that is the local frame
// of the volume
vec3 raymarch(Material volume, vec3 p, vec3 dir, vec3 light, out vec3 tr) {
    ivec3 texSize = textureSize(volume.density, 0);
    p /= texSize;
    dir /= texSize;
    dir = normalize(dir) * volume.stepsize;
    tr = vec3(0);
    // accumulated optical thickness
    vec3 k = vec3(0);
    // accumulated color
    vec3 color = vec3(0, 0, 0);
    // sigma_t for the volume, that will be modulated by the density
    vec3 sigma_t = (volume.sigma_a.rgb + volume.sigma_s.rgb) * volume.stepsize;
    // The ray marching loop
    // Max 1000 steps ...
    bool hit = false;
    for (int i=0; i<10000; ++i) {
        float d = texture(volume.density, p).r;
        if (d > 0.001) {
            hit = true;
        }
        k += d * sigma_t;
        tr = exp(-k);
        // Compute scattering at point p
        color += tr * inscatter(volume, p, dir, light) * volume.stepsize;
        // go to newt point
        p += dir;
        // Stop the computation
        if (any(greaterThan(p, vec3(1.0))) || any(lessThan(p, vec3(0.0))) || all (lessThan(tr, vec3(0.0001))))
        break;
    }
    // discard the fragment if volume is not hit
    if (!hit)
    discard;
    return color;
}

uniform Material material;
#endif
