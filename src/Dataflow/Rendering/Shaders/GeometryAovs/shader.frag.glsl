/*
the functions used to discard or shade the fragment are given by the implementation of the Material
Interface. The appropriate code will be added at runtime by the shader composer or the render pass
initialiser.
*/
#include "VertexAttribInterface.frag.glsl"

layout( location = 0 ) out vec4 out_worldpos;
layout( location = 1 ) out vec4 out_normal;

void main() {
    vec3 tc = getPerVertexTexCoord();
    // discard non opaque fragment
    vec4 bc = getBaseColor( material, tc );
    if ( toDiscard( material, bc ) ) discard;

#ifdef GLTF_MATERIAL_INTERFACE
    // Experiment on a new GLSL/Material interface allowing more efficient PBR and composition

    // Compute the normal closure. Do normal map and capture state about normal and tangent space
    NormalInfo nrm_info = getNormalInfo( material.baseMaterial, tc );
    vec3 worldNormal    = nrm_info.n;
#else
    vec3 worldNormal = getNormal(
        material, tc, getWorldSpaceNormal(), getWorldSpaceTangent(), getWorldSpaceBiTangent() );
#endif
    out_worldpos = getWorldSpacePosition();
    out_normal   = vec4( worldNormal * 0.5 + 0.5, 1.0 );
}
