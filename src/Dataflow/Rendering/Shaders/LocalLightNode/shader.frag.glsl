/*
the functions used to discard or shade the fragment are given by the implementation of the Material
Interface. The appropriate code will be added at runtime by the shader composer or the render pass
initialiser.
*/
#include "DefaultLight.glsl"
#include "VertexAttribInterface.frag.glsl"

layout( location = 5 ) in vec3 in_viewVector;
layout( location = 6 ) in vec3 in_lightVector;

uniform sampler2D amb_occ_sampler;

layout( location = 0 ) out vec4 out_color; // Position in World Space

//------------------- main ---------------------

void main() {
    /// quick access to texture coordinates
    vec3 tc = getPerVertexTexCoord();
    // discard non opaque fragment
    vec4 bc = getBaseColor( material, tc );
    if ( toDiscard( material, bc ) ) discard;

#ifdef GLTF_MATERIAL_INTERFACE
    // Experiment on a new GLSL/Material interface allowing more efficient PBR and composition

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
                                    lighting );

    // Final evaluation of BSDF/layers closure
    vec3 color = combineLayers( bsdf_params, layers, nrm_info, wo );

#else
    // All vectors are in world space
    // A material is always evaluated in the fragment local Frame
    // compute matrix from World to local Frame
    vec3 normalWorld   = getWorldSpaceNormal();    // normalized interpolated normal
    vec3 tangentWorld  = getWorldSpaceTangent();   // normalized tangent
    vec3 binormalWorld = getWorldSpaceBiTangent(); // normalized bitangent

    // Apply normal mapping
    normalWorld   = getNormal( material,
                             tc,
                             normalWorld,
                             tangentWorld,
                             binormalWorld ); // normalized bump-mapped normal
    binormalWorld = normalize( cross( normalWorld, tangentWorld ) );  // normalized tangent
    tangentWorld  = normalize( cross( binormalWorld, normalWorld ) ); // normalized bitangent

    mat3 world2local;
    world2local[0] = vec3( tangentWorld.x, binormalWorld.x, normalWorld.x );
    world2local[1] = vec3( tangentWorld.y, binormalWorld.y, normalWorld.y );
    world2local[2] = vec3( tangentWorld.z, binormalWorld.z, normalWorld.z );
    // transform all vectors in local frame so that N = (0, 0, 1);
    vec3 lightDir = world2local * normalize( in_lightVector ); // incident direction
    vec3 viewDir  = world2local * normalize( in_viewVector );  // outgoing direction

    vec3 color = evaluateBSDF( material, tc, lightDir, viewDir ) *
                 lightContributionFrom( light, getWorldSpacePosition().xyz );
#endif

    vec2 size = textureSize( amb_occ_sampler, 0 ).xy;
    vec3 ao   = texture( amb_occ_sampler, gl_FragCoord.xy / size ).rgb;

    out_color = vec4( color * ao, 1.0 );
}
