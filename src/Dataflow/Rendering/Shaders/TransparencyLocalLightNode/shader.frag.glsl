/*
the functions toDiscard and getNormals are given by the implementation of the Material Interface.
The appropriate code will be added at runtime by the shader composer or the render pass initialiser.
*/
#include "DefaultLight.glsl"
#include "VertexAttribInterface.frag.glsl"

layout( location = 0 ) out vec4 f_Accumulation;
layout( location = 1 ) out vec4 f_Revealage;

layout( location = 5 ) in vec3 in_viewVector;
layout( location = 6 ) in vec3 in_lightVector;

// uniform sampler2D amb_occ_sampler;

// implementation of weight functions of the paper
// Weighted Blended Order-Independent Transparency
// 	Morgan McGuire, Louis Bavoil - NVIDIA
// Journal of Computer Graphics Techniques (JCGT), vol. 2, no. 2, 122-141, 2013
// http://jcgt.org/published/0002/02/09/

// remark : manage only non colored transmission. Direct implementation of the above paper without
// the suggested extension :
// ... non-refractive colored transmission can be implemented as a simple extension by processing a
// separate coverage
//  value per color channel

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
    vec3 tc = getPerVertexTexCoord();
    // only render non opaque fragments and not fully transparent fragments
    vec4 bc = getBaseColor( material, tc );
    // compute the transparency factor
    float a = bc.a;
    if ( !toDiscard( material, bc ) || a < 0.001 ) discard;

#ifdef GLTF_MATERIAL_INTERFACE
    // Compute the normal closure. Do normal map and capture state about normal and tangent space
    NormalInfo nrm_info = getNormalInfo( material.baseMaterial, tc );

    // Compute the BSDF closure. Compte all view and lighting independant propserties on the
    // material
    MaterialInfo bsdf_params;
    extractBSDFParameters( material, tc, nrm_info, bsdf_params );

    // get dynamic lighting informations : wiew and light properties
    vec3 wo = normalize( in_viewVector ); // outgoing direction

    // Per local light computation
    vec3 wi       = normalize( in_lightVector ); // incident direction
    vec3 lighting = lightContributionFrom( light, getWorldSpacePosition().xyz );

    // evaluate the BSDF : get a set of layers representing several optical effects
    BsdfInfo layers = evaluateBSDF( material,
                                    bsdf_params,
                                    nrm_info,
                                    wi,
                                    wo,
                                    lightContributionFrom( light, getWorldSpacePosition().xyz ) );

    // Final evaluation of BSDF/layers closure
    vec3 color = combineLayers( bsdf_params, layers, nrm_info, wo );

#else
    // all vectors are in world space
    vec3 binormal = getWorldSpaceBiTangent();
    vec3 normalWorld =
        getNormal( material, tc, getWorldSpaceNormal(), getWorldSpaceTangent(), binormal );
    vec3 binormalWorld = normalize( cross( normalWorld, getWorldSpaceTangent() ) );
    vec3 tangentWorld  = cross( binormalWorld, normalWorld );

    // A material is always evaluated in the fragment local Frame
    // compute matrix from World to local Frame
    mat3 world2local;
    world2local[0] = vec3( tangentWorld.x, binormalWorld.x, normalWorld.x );
    world2local[1] = vec3( tangentWorld.y, binormalWorld.y, normalWorld.y );
    world2local[2] = vec3( tangentWorld.z, binormalWorld.z, normalWorld.z );
    // transform all vectors in local frame so that N = (0, 0, 1);
    vec3 wi = world2local * normalize( in_lightVector ); // incident direction
    vec3 wo = world2local * normalize( in_viewVector );  // outgoing direction

    vec3 color = evaluateBSDF( material, tc, wi, wo ) *
                 lightContributionFrom( light, getWorldSpacePosition().xyz );
#endif

    float w        = weight( gl_FragCoord.z, a );
    f_Accumulation = vec4( color * a, a ) * w;
    f_Revealage    = vec4( a );
}
