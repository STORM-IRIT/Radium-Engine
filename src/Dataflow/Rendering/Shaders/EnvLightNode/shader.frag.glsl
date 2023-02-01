/*
the functions toDiscard and getNormals are given by the implementation of the Material Interface.
The appropriate code will be added at runtime by the shader composer or the render pass initialiser.
*/

// Include the VertexAttribInterface so that all materials might be OK.
#include "VertexAttribInterface.frag.glsl"
const float OneOverPi = 0.3183098862;

layout( location = 5 ) in vec3 in_viewVector;

uniform sampler2D amb_occ_sampler;

uniform samplerCube envTexture;
uniform int numLod;

uniform mat4 redShCoeffs;
uniform mat4 greenShCoeffs;
uniform mat4 blueShCoeffs;

layout( location = 0 ) out vec4 out_color;

// For debug, to be removed once finalized.
uniform float envStrength;

//------------------- main ---------------------
// TODO : verify the computations as there is huge differences between this rendering and GLTF
// see
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/source/Renderer/shaders/ibl.glsl

// sample implementation
void main() {
    vec3 tc = getPerVertexTexCoord();
    // discard non opaque fragment
    vec4 bc = getBaseColor( material, tc );
    if ( toDiscard( material, bc ) ) discard;

#ifdef GLTF_MATERIAL_INTERFACE
    // Experiment on a new GLSL/Material interface allowing more efficient PBR and composition

    // Compute the normal closure. Do normal map and capture state about normal and tangent space
    NormalInfo nrm_info = getNormalInfo( material.baseMaterial, tc );
    vec3 view = normalize( in_viewVector );

    MaterialInfo bsdf_params;
    BsdfInfo layers;

    if ( getSeparateBSDFComponent( material,
                                   tc,
                                   view,
                                   nrm_info,
                                   bsdf_params,
                                   layers ) == 1 ) {

        vec2 size = textureSize( amb_occ_sampler, 0 ).xy;
        vec3 ao   = texture( amb_occ_sampler, gl_FragCoord.xy / size ).rgb;

        float r  = GGXroughness( bsdf_params );
        // diffuse irradiance map
        vec4 nrm = vec4( nrm_info.n, 1 );
        vec3 irradiance = vec3(
            dot( nrm, redShCoeffs * nrm ),
            dot( nrm, greenShCoeffs * nrm ),
            dot( nrm, blueShCoeffs * nrm )
        );
        layers.f_diffuse *= irradiance;

        // Specular envmap
        vec3 rfl = reflect( -view, nrm_info.n );
        layers.f_specular *=  textureLod( envTexture, rfl, r * numLod ).rgb ;

#ifdef CLEARCOAT_LAYER
        // clearcoat envmap
        r = pow(bsdf_params.clearcoat.intrough.y, 0.5);
        layers.f_clearcoat *= textureLod( envTexture, rfl, r * numLod ).rgb ;
#endif

#ifdef SHEEN_LAYER
        r = pow(bsdf_params.sheen.sheenColorRough.a, 0.5);
        layers.f_sheen *=  textureLod( envTexture, rfl, r * numLod ).rgb ;
#endif

        bc.rgb = combineLayers( bsdf_params, layers, nrm_info, view );

        bc.rgb *= ao * envStrength;
    } else {
        bc.rgb = vec3(0);
    }

#else
    vec4 normalWorld = vec4(
        getNormal(
            material, tc, getWorldSpaceNormal(), getWorldSpaceTangent(), getWorldSpaceBiTangent() ),
        1 );

    vec3 diffuse;
    vec3 specular;
    vec3 view = normalize( in_viewVector );
    vec3 rfl  = reflect( -view, normalWorld.xyz );

    if ( getSeparateBSDFComponent(
             material, getPerVertexTexCoord(), rfl, view, normalWorld.xyz, diffuse, specular ) ==
         1 )
    {
        vec2 size = textureSize( amb_occ_sampler, 0 ).xy;
        vec3 ao   = texture( amb_occ_sampler, gl_FragCoord.xy / size ).rgb;
        bc.rgb    = diffuse;
        bc.r *= dot( normalWorld, redShCoeffs * normalWorld );
        bc.g *= dot( normalWorld, greenShCoeffs * normalWorld );
        bc.b *= dot( normalWorld, blueShCoeffs * normalWorld );
        // Specular envmap
        float cosTi = clamp( dot( rfl, normalWorld.xyz ), 0.001, 1. );
        vec3 spec   = clamp( specular * cosTi * OneOverPi * 0.5, 0.001, 1. );
        float r     = getGGXRoughness( material, getPerVertexTexCoord() ) * numLod;
        bc.rgb += textureLod( envTexture, rfl, r ).rgb * spec;
        bc.rgb *= ao * envStrength;
    }
    else
    { bc.rgb = vec3( 0 ); }
#endif
    out_color = vec4( bc.rgb, 1 );
}
