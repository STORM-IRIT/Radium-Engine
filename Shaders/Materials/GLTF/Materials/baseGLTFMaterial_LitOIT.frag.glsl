layout( location = 0 ) out vec4 f_Accumulation;
layout( location = 1 ) out vec4 f_Revealage;

#include "DefaultLight.glsl"

#include "VertexAttribInterface.frag.glsl"
layout( location = 5 ) in vec3 in_viewVector;
layout( location = 6 ) in vec3 in_lightVector;

// -----------------------------------------------------------

// implementation of weight functions of the paper
// Weighted Blended Order-Independent Transparency
// 	Morgan McGuire, Louis Bavoil - NVIDIA
// Journal of Computer Graphics Techniques (JCGT), vol. 2, no. 2, 122-141, 2013
// http://jcgt.org/published/0002/02/09/

// remark : manage only non colored transmission. see the paper for :
// ... non-refractive colored transmission can be implemented as a simple extension by processing a
// separate coverage value per color channel

// Note, z range from 0 at the camera to +infinity far away ...

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
#ifdef MATERIAL_UNLIT
    discard;
#else
    vec3 tc = getPerVertexTexCoord();
    // only render non opaque fragments and not fully transparent fragments
    vec4 bc = getBaseColor( material, tc );
    // compute the transparency factor
    float a = bc.a;
    if ( !toDiscard( material, bc ) || a < 0.001 ) discard;
    NormalInfo nrm_info = getNormalInfo( material.baseMaterial, tc );

    MaterialInfo bsdf_params;
    extractBSDFParameters( material, tc, nrm_info, bsdf_params );
    vec3 wo = normalize( in_viewVector ); // outgoing direction

    // the following could be done for each ligh source (in a loop) ...

    vec3 wi         = normalize( in_lightVector ); // incident direction
    BsdfInfo layers = evaluateBSDF( material, bsdf_params,
                                    nrm_info,
                                    wi,
                                    wo,
                                    lightContributionFrom( light, getWorldSpacePosition().xyz ) );

    vec3 color = combineLayers( bsdf_params, layers, nrm_info, wo );

    float w        = weight( gl_FragCoord.z, a );
    f_Accumulation = vec4( color * a, a ) * w;
    f_Revealage    = vec4( a );
#endif
}
