#ifndef SPECULARGLOSSINESS_GLSL
#define SPECULARGLOSSINESS_GLSL
#include "baseGLTFMaterial.glsl"

struct Material {
    GLTFBaseMaterial baseMaterial;
    vec4 diffuseFactor;
    vec4 specularFactor;
    float glossinessFactor;
#ifdef TEXTURE_DIFFUSE
    sampler2D diffuse;
#    ifdef TEXTURE_COORD_TRANSFORM_DIFFUSE
    mat3 diffuseTransform;
#    endif
#endif
#ifdef TEXTURE_SPECULARGLOSSINESS
    sampler2D specularGlossiness;
#    ifdef TEXTURE_COORD_TRANSFORM_SPECULARGLOSSINESS
    mat3 specularGlossinessTransform;
#    endif
#endif
};

bool toDiscard( Material material, vec4 color ) {
    return toDiscardBase( material.baseMaterial, color );
}

vec3 getEmissiveColor( Material material, vec3 textCoord ) {
    return getEmissiveColorBase( material.baseMaterial, textCoord );
}

vec4 getBaseColor( Material material, vec3 texCoord ) {
    vec4 basecolor = material.diffuseFactor;
#ifdef TEXTURE_DIFFUSE
#    ifdef TEXTURE_COORD_TRANSFORM_DIFFUSE
    vec2 ct = ( material.diffuseTransform * vec3( texCoord.xy, 1 ) ).xy;
#    else
    vec2 ct = texCoord.xy;
#    endif
    basecolor *= texture( material.diffuse, ct );
#endif
#ifdef HAS_PERVERTEX_COLOR
    return basecolor * getPerVertexBaseColor();
#else
    return basecolor;
#endif
}

// Specular and glossiness are packed scalars
// Following GLTF specification,  Specular is stored in the 'rgb' channel, glossiness is stored in
// the 'a' channel. The 'a' channel of the returned color is the computed alphaRoughness of the BSDF
// model
vec4 getSpecularGlossinessFactor( Material material, vec2 tc ) {
    vec4 specGloss = vec4( material.specularFactor.rgb, material.glossinessFactor );
#ifdef TEXTURE_SPECULARGLOSSINESS
#    ifdef TEXTURE_COORD_TRANSFORM_SPECULARGLOSSINESS
    vec2 ct = ( material.specularGlossinessTransform * vec3( tc, 1 ) ).xy;
#    else
    vec2 ct = tc;
#    endif
    specGloss *= texture( material.specularGlossiness, ct );
#endif
    // convert glossinesss to the alpha roughness (alpha = (1-glossiness)^2
    specGloss.a = ( 1 - specGloss.a );
    clamp( specGloss.a, c_MinRoughness, 1 );
    return specGloss;
}

int extractBSDFParameters( Material material, vec3 tc, NormalInfo N, inout MaterialInfo params ) {
    params.basecolor          = getBaseColor( material, tc ).rgb;
    vec4 spec_gloss           = getSpecularGlossinessFactor( material, tc.xy );
    params.f0                 = spec_gloss.rgb;
    params.f90                = vec3( 1 ); // f90( wich color to use ?)
    params.RoughnessMetalness = vec3( spec_gloss.a * spec_gloss.a, spec_gloss.a, 0. );
    params.diffusebase =
        params.basecolor * ( 1. - max( max( params.f0.r, params.f0.g ), params.f0.b ) );
    // In case an extension want to modify the material data ...
    return extractBSDFParametersBase( material.baseMaterial, tc, N, params );
}

int getSeparateBSDFComponent( Material material,
                              vec3 tc,
                              vec3 V,
                              NormalInfo N,
                              inout MaterialInfo params,
                              out BsdfInfo layers ) {
    if ( extractBSDFParameters( material, tc, N, params ) == 1 )
    { return getSeparateBSDFComponentBase( material.baseMaterial, params, N, V, layers ); }
    else
    { return 0; }
}

BsdfInfo evaluateBSDF( Material material,
                       MaterialInfo bsdf_params,
                       NormalInfo N,
                       vec3 wi,
                       vec3 wo,
                       vec3 light_intensity ) {
    return evaluateBSDFBase( material.baseMaterial, bsdf_params, N, wi, wo, light_intensity );
}

uniform Material material;

#endif // SPECULARGLOSSINESS_GLSL
