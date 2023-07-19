// This is the basic fragmentShader any PBR material can use.
// A specific material fragment shader implements the material interface (computeMaterialInternal)
// and include this shader

// This is for a preview of the shader composition, but in time we must use more specific Light
// Shader
#include "DefaultLight.glsl"

out vec4 fragColor;

#include "VertexAttribInterface.frag.glsl"
// -----------------
layout( location = 5 ) in vec3 in_viewVector;
layout( location = 6 ) in vec3 in_lightVector;

void main() {
    vec3 tc = getPerVertexTexCoord();
    // discard non opaque fragment
    vec4 bc = getBaseColor( material, tc );
    if ( toDiscard( material, bc ) ) discard;

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
#ifdef USE_IBL
    color = modulateByAO( material.baseMaterial, color, getPerVertexTexCoord() );
#endif
    // color      = color + getEmissiveColor(material.baseMaterial, getPerVertexTexCoord());
    fragColor = vec4( color, 1.0 );
}
