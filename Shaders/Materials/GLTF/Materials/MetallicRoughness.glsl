#ifndef METALLICROUGHNESS_GLSL
#define METALLICROUGHNESS_GLSL
#include "baseGLTFMaterial.glsl"

struct Material {
    GLTFBaseMaterial baseMaterial;
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
#ifdef TEXTURE_BASECOLOR
    sampler2D baseColor;
#    ifdef TEXTURE_COORD_TRANSFORM_BASECOLOR
    mat3 baseTransform;
#    endif
#endif
#ifdef TEXTURE_METALLICROUGHNESS
    sampler2D metallicRoughness;
#    ifdef TEXTURE_COORD_TRANSFORM_METALLICROUGHNESS
    mat3 metallicRoughnessTransform;
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
    vec4 basecolor = material.baseColorFactor;

#ifdef TEXTURE_BASECOLOR
    vec2 tc = texCoord.xy;
#    ifdef TEXTURE_COORD_TRANSFORM_BASECOLOR
    tc = ( material.baseTransform * vec3( tc, 1 ) ).xy;
#    endif
    basecolor *= texture( material.baseColor, tc );
#endif
#ifdef HAS_PERVERTEX_COLOR
    basecolor *= getPerVertexBaseColor();
#endif
    if ( material.baseMaterial.alphaMode == 1 && basecolor.a < material.baseMaterial.alphaCutoff ) {
        basecolor.a = 0;
    }
    return basecolor;
}

// Metallic and roughness are packed scalars
// Following GLTF specification,  Roughness is stored in the 'g' channel, metallic is stored in the
// 'b' channel. The 'r' channel is ignored for MetallicRougness by gltf, we use it to store the
// alphaRoughness
vec3 getMetallicRoughnessFactors( Material material, vec2 tc ) {
    vec3 mr = vec3( 0, material.roughnessFactor, material.metallicFactor );
#ifdef TEXTURE_METALLICROUGHNESS
#    ifdef TEXTURE_COORD_TRANSFORM_METALLICROUGHNESS
    vec2 ct = ( material.metallicRoughnessTransform * vec3( tc, 1 ) ).xy;
#    else
    vec2 ct = tc;
#    endif
    vec3 mrsample = texture( material.metallicRoughness, ct ).rgb;
    mr            = mr * mrsample;
#endif
    clamp( mr, vec3( 0, c_MinRoughness, 0 ), vec3( 0, 1, 1 ) );
    mr.r = mr.g * mr.g;
    return mr;
}

int extractBSDFParameters( Material material, vec3 tc, NormalInfo N, inout MaterialInfo params ) {
    params.basecolor          = getBaseColor( material, tc ).rgb;
    params.f0                 = vec3( dielectricSpecular( material.baseMaterial.ior ) );
    params.f90                = vec3( 1 ); // f90( wich color to use ?)
    params.RoughnessMetalness = getMetallicRoughnessFactors( material, tc.xy );
    params.diffusebase        = mix( params.basecolor, vec3( 0 ), params.RoughnessMetalness.b );
    params.f0                 = mix( params.f0, params.basecolor, params.RoughnessMetalness.b );
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
    {
        return getSeparateBSDFComponentBase(
            material.baseMaterial, params, N, V, layers );
    }
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

#endif // METALLICROUGHNESS_GLSL
