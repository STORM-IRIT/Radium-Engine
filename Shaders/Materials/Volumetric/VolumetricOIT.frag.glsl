// Fragment position in model space
layout( location = 0 ) in vec3 in_position;
layout( location = 2 ) in vec3 in_texcoord;
// view position in model space
layout( location = 5 ) in vec3 in_eyeInModelSpace;
layout( location = 6 ) in vec3 in_lightVector;

// The modeltoimage matrix
in mat4 biasmvp;
// the depth and color texture whre the volume is rendered.
uniform sampler2D imageColor;
uniform sampler2D imageDepth;

#include "Volumetric.glsl"

layout( location = 0 ) out vec4 f_Accumulation;
layout( location = 1 ) out vec4 f_Revealage;

float weight( float z, float alpha ) {
    // pow(alpha, colorResistance) : increase colorResistance if foreground transparent are
    // affecting background transparent color clamp(adjust / f(z), min, max) :
    //     adjust : Range adjustment to avoid saturating at the clamp bounds
    //     clamp bounds : to be tuned to avoid over or underflow of the reveleage texture.
    // f(z) = 1e-5 + pow(z/depthRange, orederingStrength)
    //     defRange : Depth range over which significant ordering discrimination is required. Here,
    //     10 camera space units.
    //         Decrease if high-opacity surfaces seem “too transparent”,
    //         increase if distant transparents are blending together too much.
    //     orderingStrength : Ordering strength. Increase if background is showing through
    //     foreground too much.
    // 1e-5 + ... : avoid dividing by zero !

    return pow( alpha, 0.5 ) * clamp( 10 / ( 1e-5 + pow( z / 10, 6 ) ), 1e-2, 3 * 1e3 );
}

void main() {

    // Transform all informations in the canonical grid space
    vec4 pos = material.modelToDensity * vec4( in_position, 1 );
    vec4 eye = material.modelToDensity * vec4( in_eyeInModelSpace, 1 );

    // compute the ray dir in model space
    vec3 rayDir = normalize( pos.xyz - eye.xyz );

    vec3 volTransmitance;
    vec3 volColor =
        raymarch( material, in_position, rayDir, in_lightVector, biasmvp, volTransmitance );
    // Take into account the existing color modulated by the trasmittance
    // vec3 backgroungColor = volTransmitance * existingfragmentcolor;
    float a        = 1 - length( volTransmitance );
    float w        = weight( gl_FragCoord.z, a );
    f_Accumulation = vec4( volColor * a, a ) * w;
    f_Revealage    = vec4( a );
}
