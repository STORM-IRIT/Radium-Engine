#ifndef GLTF_BASEMATERIAL_GLSL
#define GLTF_BASEMATERIAL_GLSL

// new version of the GLTF material implementation
// ----------------------------------------------------
// ----------------------------------------------------
#include "VertexAttribInterface.frag.glsl"

/*
 * https://github.com/KhronosGroup/glTF-Sample-Viewer/tree/master/source/Renderer/shaders
 *
 * Partly update with specification of 12/2021
 * https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation
 */

/*
 * Define constant, datastructure and operators used for all pbr materials of a GLTF scene
 */

const float M_PI  = 3.141592653589793;
const float InvPi = 0.31830988618379067154;

const float local_epsilon  = 1e-6;
const float c_MinRoughness = 0.001;

float clampedDot( vec3 x, vec3 y ) {
    // clamp to [local_epsilon, 1 ) ?
    return clamp( dot( x, y ), 0.0, 1.0 );
}

vec3 F_Schlick( vec3 f0, vec3 f90, float VdotH ) {
    return f0 + ( f90 - f0 ) * pow( clamp( 1.0 - VdotH, 0.0, 1.0 ), 5.0 );
}

float V_GGX( float NdotL, float NdotV, float alphaRoughness ) {
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;

    float GGXV = NdotL * sqrt( NdotV * NdotV * ( 1.0 - alphaRoughnessSq ) + alphaRoughnessSq );
    float GGXL = NdotV * sqrt( NdotL * NdotL * ( 1.0 - alphaRoughnessSq ) + alphaRoughnessSq );

    float GGX = GGXV + GGXL;
    if ( GGX > 0.0 ) { return 0.5 / GGX; }
    return 0.0;
}

float D_GGX( float NdotH, float alphaRoughness ) {
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;
    float f                = ( NdotH * NdotH ) * ( alphaRoughnessSq - 1.0 ) + 1.0;
    return alphaRoughnessSq / ( M_PI * f * f );
}

vec3 BRDF_lambertian( vec3 diffuseColor, float VdotH, vec3 f0, vec3 f90, float specularWeight ) {
    // see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    return ( 1.0 - specularWeight * F_Schlick( f0, f90, VdotH ) ) * ( diffuseColor / M_PI );
}

vec3 BRDF_specularGGX( vec3 f0,
                       vec3 f90,
                       float alphaRoughness,
                       float specularWeight,
                       float VdotH,
                       float NdotL,
                       float NdotV,
                       float NdotH ) {
    vec3 F    = F_Schlick( f0, f90, VdotH );
    float Vis = V_GGX( NdotL, NdotV, alphaRoughness );
    float D   = D_GGX( NdotH, alphaRoughness );

    return specularWeight * F * Vis * D;
}

/* -- Layer extensions -- */
#ifdef CLEARCOAT_LAYER
// definition of the clearcoat layer
struct Clearcoat {
    float clearcoatFactor;
    float clearcoatRoughnessFactor;
#    ifdef TEXTURE_CLEARCOAT
    sampler2D clearcoatTexture;
#        ifdef TEXTURE_COORD_TRANSFORM_CLEARCOAT
    mat3 clearcoatTextureTransform;
#        endif
#    endif
#    ifdef TEXTURE_CLEARCOATROUGHNESS
    sampler2D clearcoatRoughnessTexture;
#        ifdef TEXTURE_COORD_TRANSFORM_CLEARCOATROUGHNESS
    mat3 clearcoatRoughnessTextureTransform;
#        endif
#    endif
#    ifdef TEXTURE_CLEARCOATNORMAL
    float clearcoatNormalTextureScale;
    sampler2D clearcoatNormalTexture;
#        ifdef TEXTURE_COORD_TRANSFORM_CLEARCOATNORMAL
    mat3 clearcoatNormalTextureTransform;
#        endif
#    endif
};
#endif

#ifdef SPECULAR_LAYER
struct Specular {
    float specularFactor;
    vec4 specularColorFactor;
#    ifdef TEXTURE_SPECULAR_EXT
    sampler2D specularTexture;
#        ifdef TEXTURE_COORD_TRANSFORM_SPECULAR_EXT
    mat3 specularTextureTransform;
#        endif
#    endif
#    ifdef TEXTURE_SPECULARCOLOR_EXT
    sampler2D specularColorTexture;
#        ifdef TEXTURE_COORD_TRANSFORM_SPECULARCOLOR_EXT
    mat3 specularColorTextureTransform;
#        endif
#    endif
};
#endif

#ifdef SHEEN_LAYER
struct Sheen {
    sampler2D sheenE_LUT;
    sampler2D charlieLUT;

    float sheenRoughnessFactor;
    vec4 sheenColorFactor;

#    ifdef TEXTURE_SHEEN_COLOR
    sampler2D sheenColorTexture;
#        ifdef TEXTURE_COORD_TRANSFORM_SHEEN_COLOR
    mat3 sheenColorTextureTransform;
#        endif
#    endif

#    ifdef TEXTURE_SHEEN_ROUGHNESS
    sampler2D sheenRoughnessTexture;
#        ifdef TEXTURE_COORD_TRANSFORM_SHEEN_ROUGHNESS
    mat3 sheenRoughnessTextureTransform;
#        endif
#    endif
};
#endif

/* -- Base material -- */
struct GLTFBaseMaterial {
    vec4 emissiveFactor;
    uint alphaMode; // 0 --> Opaque, 1 --> Mask, 2 --> Blend
    int doubleSided;
    float alphaCutoff;
    float ior;
#ifdef TEXTURE_NORMAL
    float normalTextureScale;
    sampler2D normal;
#    ifdef TEXTURE_COORD_TRANSFORM_NORMAL
    mat3 normalTransform;
#    endif
#endif
#ifdef TEXTURE_OCCLUSION
    float occlusionStrength;
    sampler2D occlusion;
#    ifdef TEXTURE_COORD_TRANSFORM_OCCLUSION
    mat3 occlusionTransform;
#    endif
#endif
#ifdef TEXTURE_EMISSIVE
    sampler2D emissive;
#    ifdef TEXTURE_COORD_TRANSFORM_EMISSIVE
    mat3 emmissiveTransform;
#    endif
#endif
#ifdef CLEARCOAT_LAYER
    Clearcoat clearcoat;
#endif
#ifdef SPECULAR_LAYER
    Specular specular;
#endif
#ifdef SHEEN_LAYER
    Sheen sheen;
#endif
    // Global pre-computed data for BSDF evaluation
    sampler2D ggxLut;
};

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in these struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.

// Based on :
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/source/Renderer/shaders/material_info.glsl
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/source/Renderer/shaders/pbr.frag

/*
 * Definestructure and functions to access wormal and world-space to local space transformation
 * mapping
 */
// the struct Normal info contains all vectors in world space
struct NormalInfo {
    vec3 ng; // Geometry normal
    vec3 t;  // Geometry tangent
    vec3 b;  // Geometry bitangent
    vec3 n;  // Shading normal
};

NormalInfo getNormalInfo( GLTFBaseMaterial material, vec3 texCoord ) {
    // if tangent are usable, just get the interpolated TBN matrix.
    // For now, compute it.
    NormalInfo res;
    res.ng = getWorldSpaceNormal();
    res.t  = getWorldSpaceTangent();
    res.b  = cross( res.ng, res.t );
    // For a back-facing surface, the tangential basis vectors are negated.
    if ( gl_FrontFacing == false )
    {
        res.t *= -1.0;
        res.b *= -1.0;
        res.ng *= -1.0;
    }
#ifdef TEXTURE_NORMAL
    vec2 ct = texCoord.xy;
#    ifdef TEXTURE_COORD_TRANSFORM_NORMAL
    ct = ( material.normalTransform * vec3( ct, 1 ) ).xy;
#    endif
    res.n = texture( material.normal, ct ).rgb * 2.0 - 1.0;
    res.n *= vec3( material.normalTextureScale, material.normalTextureScale, 1.0 );
    res.n = normalize( res.n );
#    ifdef SHADER_DEBUG
    res.ntex = res.n;
#    endif
    res.n = normalize( mat3( res.t, res.b, res.ng ) * res.n );
#else
    res.n     = res.ng;
#endif
    return res;
}

#ifdef CLEARCOAT_LAYER
struct ClearcoatInfo {
    vec3 f0;       // will be the same as the base layer f0
    vec3 f90;      // set to 1 in the spec and ref implementation
    vec2 intrough; // x contains the clearcoat intensity, y its roughness
    vec3 normal;   // contains the normal of the clearcoat layer
};

ClearcoatInfo getClearcoatInfo( Clearcoat u_clearcoat, NormalInfo nrm, vec3 f0, vec3 texCoord ) {
    ClearcoatInfo res;
    res.intrough = vec2( u_clearcoat.clearcoatFactor, u_clearcoat.clearcoatRoughnessFactor );
    res.f0       = f0;
    res.f90      = vec3( 1. );

    vec2 ct;
#    ifdef TEXTURE_CLEARCOAT
#        ifdef TEXTURE_COORD_TRANSFORM_CLEARCOAT
    ct = ( u_clearcoat.clearcoatTextureTransform * texCoord ).xy;
#        else
    ct = texCoord.xy;
#        endif
    res.intrough.x *= texture( u_clearcoat.clearcoatTexture, ct ).r;
#    endif

#    ifdef TEXTURE_CLEARCOATROUGHNESS
#        ifdef TEXTURE_COORD_TRANSFORM_CLEARCOATROUGHNESS
    ct = ( u_clearcoat.clearcoatRoughnessTextureTransform * texCoord ).xy;
#        else
    ct = texCoord.xy;
#        endif
    res.intrough.y *= texture( u_clearcoat.clearcoatRoughnessTexture, ct ).g;
#    endif

#    ifdef TEXTURE_CLEARCOATNORMAL
#        ifdef TEXTURE_COORD_TRANSFORM_CLEARCOATNORMAL
    ct = ( u_clearcoat.clearcoatNormalTextureTransform * texCoord ).xy;
#        else
    ct = texCoord.xy;
#        endif
    res.normal = normalize( ( texture( u_clearcoat.clearcoatNormalTexture, ct ).rgb * 2 - 1 ) *
                            vec3( u_clearcoat.clearcoatNormalTextureScale,
                                  u_clearcoat.clearcoatNormalTextureScale,
                                  1 ) );
    res.normal = mat3( nrm.t, nrm.b, nrm.ng ) * res.normal;
#    else
    res.normal = nrm.ng;
#    endif
    res.intrough.y = clamp( res.intrough.y, 0., 1. );
    return res;
}

vec3 getPunctualRadianceClearCoat( ClearcoatInfo clearcoat, vec3 v, vec3 l, vec3 h, float vdoth ) {
    float ndotl = clampedDot( clearcoat.normal, l );
    float ndotv = clampedDot( clearcoat.normal, v );
    float ndoth = clampedDot( clearcoat.normal, h );
    return ndotl * BRDF_specularGGX( clearcoat.f0,
                                     clearcoat.f90,
                                     clearcoat.intrough.y * clearcoat.intrough.y,
                                     1.,
                                     vdoth,
                                     ndotl,
                                     ndotv,
                                     ndoth );
}
#endif

#ifdef SHEEN_LAYER
struct SheenInfo {
    // rgb --> sheen color; a --> sheen roughness
    vec4 sheenColorRough;
};

SheenInfo getSheenInfo( Sheen u_sheen, vec3 texCoord ) {
    SheenInfo res;
    res.sheenColorRough.rgb = u_sheen.sheenColorFactor.rgb;
    res.sheenColorRough.a   = u_sheen.sheenRoughnessFactor;
    vec2 tc;
#    ifdef TEXTURE_SHEEN_COLOR
#        ifdef TEXTURE_COORD_TRANSFORM_SHEEN_COLOR
    tc = ( u_sheen.sheenColorTextureTransform * texCoord ).xy;
#        else
    tc = texCoord.xy;
#        endif
    res.sheenColorRough.rgb *= texture( u_sheen.sheenColorTexture, tc ).rgb;
#    endif

#    ifdef TEXTURE_SHEEN_ROUGHNESS
#        ifdef TEXTURE_COORD_TRANSFORM_SHEEN_ROUGHNESS
    tc = ( u_sheen.sheenRoughnessTextureTransform * texCoord ).xy;
#        else
    tc = texCoord.xy;
#        endif
    res.sheenColorRough.a *= texture( u_sheen.sheenRoughnessTexture, tc ).a;
#    endif
    res.sheenColorRough.a = clamp( res.sheenColorRough.a, 0., 1. );
    return res;
}

float albedoSheenScalingLUT( Sheen u_sheen, float NdotV, float roughness ) {
    return texture(u_sheen.sheenE_LUT, vec2(NdotV, roughness)).r;
}

float D_Charlie( float roughness, float NdotH ) {
    float alphaG = roughness * roughness;
    float invR   = 1.0 / alphaG;
    float cos2h  = NdotH * NdotH;
    float sin2h  = 1.0 - cos2h;
    return ( 2.0 + invR ) * pow( sin2h, invR * 0.5 ) / ( 2.0 * M_PI );
}

float lambdaSheenNumericHelper( float x, float alphaG ) {
    float oneMinusAlphaSq = ( 1.0 - alphaG ) * ( 1.0 - alphaG );
    float a               = mix( 21.5473, 25.3245, oneMinusAlphaSq );
    float b               = mix( 3.82987, 3.32435, oneMinusAlphaSq );
    float c               = mix( 0.19823, 0.16801, oneMinusAlphaSq );
    float d               = mix( -1.97760, -1.27393, oneMinusAlphaSq );
    float e               = mix( -4.32054, -4.85967, oneMinusAlphaSq );
    return a / ( 1.0 + b * pow( x, c ) ) + d * x + e;
}

float lambdaSheen( float cosTheta, float alphaG ) {
    if ( abs( cosTheta ) < 0.5 ) { return exp( lambdaSheenNumericHelper( cosTheta, alphaG ) ); }
    else
    {
        return exp( 2.0 * lambdaSheenNumericHelper( 0.5, alphaG ) -
                    lambdaSheenNumericHelper( 1.0 - cosTheta, alphaG ) );
    }
}

// according to the spec, this could be optimized using the "Aschikmin approximation"
// see the following :
// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_sheen
// https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec-2021x.md.html#components/sheen
float V_Sheen( float NdotL, float NdotV, float roughness ) {
    // This is the "Charlie", full precision visibility
    roughness    = max( roughness, 0.000001 ); // clamp (0,1]
    float alphaG = roughness * roughness;

    return clamp( 1.0 / ( ( 1.0 + lambdaSheen( NdotV, alphaG ) + lambdaSheen( NdotL, alphaG ) ) *
                          ( 4.0 * NdotV * NdotL ) ),
                  0.0,
                  1.0 );
}

vec3 BRDF_specularSheen( SheenInfo params, float NdotL, float NdotV, float NdotH ) {
    float sheenDistribution = D_Charlie( params.sheenColorRough.a, NdotH );
    float sheenVisibility   = V_Sheen( NdotL, NdotV, params.sheenColorRough.a );
    return params.sheenColorRough.rgb * sheenDistribution * sheenVisibility;
}

vec3 getPunctualRadianceSheen( SheenInfo params, float NdotL, float NdotV, float NdotH ) {
    return NdotL * BRDF_specularSheen( params, NdotL, NdotV, NdotH );
}

float max3( vec3 v ) {
    return max( max( v.x, v.y ), v.z );
}

#endif
struct MaterialInfo {
    vec3 basecolor;
    vec3 f0;  // full reflectance color at normal incidence angle. Computed from ior for dielectric
    vec3 f90; // full reflectance at grazing angle. Set to 1. in the spec.
    vec3 diffusebase; // diffuse base coefficient
    // r contains alphaRoughness, g contains perceptual roughness, b contains metallic coefficient
    vec3 RoughnessMetalness;
    // set to 1 by the spec, can bve modified by extension KHR_materials_specular
    float specularWeight;
#ifdef SHEEN_LAYER
    SheenInfo sheen;
#endif
#ifdef CLEARCOAT_LAYER
    ClearcoatInfo clearcoat;
#endif
    // add this when KHR_materials_specular extension will be supported
    // float specularWeight
};

#ifdef SPECULAR_LAYER
void getSpecularInfo( inout MaterialInfo base,
                      Specular u_specular,
                      NormalInfo nrm,
                      vec3 texCoord ) {
    vec4 specularTexture = vec4( 1.0 );
    vec2 ct              = texCoord.xy;
#    ifdef TEXTURE_SPECULAR_EXT
#        ifdef TEXTURE_COORD_TRANSFORM_SPECULAR_EXT
    ct = ( u_specular.specularTextureTransform * texCoord ).xy;
#        endif
    specularTexture.a = texture( u_specular.specularTexture, ct ).a;
#    endif

#    ifdef TEXTURE_SPECULARCOLOR_EXT
#        ifdef TEXTURE_COORD_TRANSFORM_SPECULARCOLOR_EXT
    ct = ( u_specular.specularColorTextureTransform * texCoord ).xy;
#        else
    ct = texCoord.xy;
#        endif
    specularTexture.rgb = texture( u_specular.specularColorTexture, ct ).rgb;
#    endif

    vec3 dielectricSpecularF0 =
        min( base.f0 * u_specular.specularColorFactor.rgb * specularTexture.rgb, vec3( 1.0 ) );
    base.f0             = mix( dielectricSpecularF0, base.basecolor, base.RoughnessMetalness.b );
    base.specularWeight = u_specular.specularFactor * specularTexture.a;
}
#endif

bool toDiscardBase( GLTFBaseMaterial material, vec4 color ) {
    if ( material.alphaMode == 1 && color.a < material.alphaCutoff ) return true;
    if ( material.alphaMode == 2 && color.a < 1 ) return true;
    return false;
}

float dielectricSpecular( float ior ) { // compute f0
    float rel_ior = ( ior - 1 ) / ( ior + 1 );
    return rel_ior * rel_ior;
}

#define RESPECT_SPEC
// compute F90 coef according to material spec (GLTF says 1, Schlick says what is in the comment)
vec3 f90( vec3 specularColor ) {
#ifdef RESPECT_SPEC
    return vec3( 1 );
#else
    float r   = max( max( specularColor.r, specularColor.g ), specularColor.b );
    float r90 = clamp( r * 25.0, 0.0, 1.0 );
    return vec3( r90 );
#endif
}

vec3 getEmissiveColorBase( GLTFBaseMaterial material, vec3 textCoord ) {
    vec3 e = material.emissiveFactor.rgb;
#ifdef TEXTURE_EMISSIVE
#    ifdef TEXTURE_COORD_TRANSFORM_EMISSIVE
    vec3 ct = material.emissiveTransform * vec3( textCoord.xy, 1 );
#    else
    vec3 ct = vec3( textCoord.xy, 1 );
#    endif
    e *= texture( material.emissive, ct.xy ).rgb;
#endif
    return e;
}

int extractBSDFParametersBase( GLTFBaseMaterial material,
                               vec3 tc,
                               NormalInfo N,
                               inout MaterialInfo params ) {
    params.specularWeight = 1.0;
#ifdef SHEEN_LAYER
    // prepare SHEEN material info
    params.sheen = getSheenInfo( material.sheen, vec3( tc.xy, 1 ) );
#endif
#ifdef CLEARCOAT_LAYER
    params.clearcoat = getClearcoatInfo( material.clearcoat, N, params.f0, vec3( tc.xy, 1 ) );
#endif
#ifdef SPECULAR_LAYER
    getSpecularInfo( params, material.specular, N, vec3( tc.xy, 1 ) );
#endif
#ifdef MATERIAL_TRANSMISSION
    getTransmissionInfo( params );
#endif
#ifdef MATERIAL_VOLUME
    getVolumeInfo( params );
#endif
    return 1;
}
/// Implementation of separable BSDF interface
/// BSDF SEPARABLE INTERFACE

struct BsdfInfo {
    vec3 f_diffuse;
    vec3 f_specular;
#ifdef CLEARCOAT_LAYER
    vec3 f_clearcoat;
#endif
#ifdef SHEEN_LAYER
    vec3 f_sheen;
    float sheen_scaling;
#endif
    // add other layers
};

// verify this ... why r^.5 ?
float GGXroughness( MaterialInfo params ) {
    return pow( params.RoughnessMetalness.g, 0.5 );
}

BsdfInfo evaluateBSDFBase( GLTFBaseMaterial material,
                           MaterialInfo bsdf_params,
                           NormalInfo N,
                           vec3 wi, // L
                           vec3 wo, // N
                           vec3 light_intensity ) {
    BsdfInfo result;
    vec3 ns     = N.n;
    float cosTo = dot( wo, ns );
    /* If material is double side, disable culling in the renderer
    if ( material.doubleSided == 1 )
    {
        if ( cosTo < 0. )
        {
            // back face fragment
            ns *= -1;
            cosTo = -cosTo;
        }
    }
    else
    { cosTo = clamp( cosTo, 0.0, 1.0 ); }
     */

    cosTo = clamp( cosTo, 0.0, 1.0 );

    // Just in case of null lighting direction, consider normal incidence
    if ( length( wi ) < local_epsilon ) wi = ns;

    vec3 h = wi + wo;
    if ( length( h ) < local_epsilon )
        h = ns; // TODO --> this could be buggy
    else
        h = normalize( h );

    float cosTi = clampedDot( ns, wi );

    if ( cosTo > 0 || cosTi > 0 )
    {
        float VdotH = clampedDot( wo, h );
        float NdotH = clampedDot( ns, h );

        result.f_diffuse = light_intensity * cosTi *
                           BRDF_lambertian( bsdf_params.diffusebase,
                                            VdotH,
                                            bsdf_params.f0,
                                            bsdf_params.f90,
                                            bsdf_params.specularWeight );
        result.f_specular = light_intensity * cosTi *
                            BRDF_specularGGX( bsdf_params.f0,
                                              bsdf_params.f90,
                                              bsdf_params.RoughnessMetalness.r,
                                              bsdf_params.specularWeight,
                                              VdotH,
                                              cosTi,
                                              cosTo,
                                              NdotH );
#ifdef CLEARCOAT_LAYER
        result.f_clearcoat = light_intensity * getPunctualRadianceClearCoat(
                                                   bsdf_params.clearcoat, wo, wi, h, VdotH );
#endif

#ifdef SHEEN_LAYER
        result.f_sheen =
            light_intensity * getPunctualRadianceSheen( bsdf_params.sheen, cosTi, cosTo, NdotH );
        float scalingV =
            albedoSheenScalingLUT( material.sheen, cosTo, bsdf_params.sheen.sheenColorRough.a );
        float scalingL =
            albedoSheenScalingLUT( material.sheen, cosTi, bsdf_params.sheen.sheenColorRough.a );
        result.sheen_scaling =
            min( 1.0 - max3( bsdf_params.sheen.sheenColorRough.rgb ) * scalingV,
                 1.0 - max3( bsdf_params.sheen.sheenColorRough.rgb ) * scalingL );
        // DEBUG
        // result.sheen_scaling = bsdf_params.sheen.sheenColorRough.a;
#endif
    }

    return result;
}

// will not work if there is more than one light in the scene. clearcoat must be applied once and
// not after each light pass
vec3 combineLayers( MaterialInfo mat, BsdfInfo bsdf, NormalInfo N, vec3 v ) {
#ifdef CLEARCOAT_LAYER
    vec3 ccfr =
        F_Schlick( mat.clearcoat.f0, mat.clearcoat.f90, clampedDot( mat.clearcoat.normal, v ) );
    bsdf.f_clearcoat *= mat.clearcoat.intrough.x;
#endif
    // modify the following each time a layer extension is added
    vec3 color = bsdf.f_specular + bsdf.f_diffuse;
#ifdef SHEEN_LAYER
    color = color * bsdf.sheen_scaling + bsdf.f_sheen;
#endif
#ifdef CLEARCOAT_LAYER
    color = color * ( 1.0 - mat.clearcoat.intrough.r * ccfr ) + bsdf.f_clearcoat;
#endif
    return color;
}

// 496
int getSeparateBSDFComponentBase( GLTFBaseMaterial material,
                                  MaterialInfo params,
                                  NormalInfo N,
                                  vec3 wo,
                                  out BsdfInfo layers ) {
    vec3 ns     = N.n;
    float cosTo = dot( wo, ns );
    // Will be set to 1 if base layers (diffuse + specular) are computed
    int result = 0;

    /* If material is double side, disable culling in the renderer
    if ( material.doubleSided == 1 )
    {
        if ( cosTo < 0. )
        {
            // back face fragment
            ns *= -1;
            cosTo = -cosTo;
        }
    }
    else
    { cosTo = clamp( cosTo, 0.0, 1.0 ); }
     */

    cosTo = clamp( cosTo, 0.0, 1.0 );
    if ( cosTo > 0 )
    {
        float r   = GGXroughness( params );
        vec2 f_ab = texture( material.ggxLut, vec2( cosTo, r ) ).rg;
        // see
        // https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/source/Renderer/shaders/ibl.glsl
        // see https://bruop.github.io/ibl/#single_scattering_results
        vec3 Fr     = max( vec3( 1. - r ), params.f0 ) - params.f0;
        vec3 Ks     = params.f0 + Fr * pow( 1. - cosTo, 5.0 );
        vec3 FssEss = params.specularWeight * (Ks * f_ab.x + f_ab.y);
        layers.f_specular    =  FssEss;

        float Ems   = ( 1.0 - ( f_ab.x + f_ab.y ) );
        vec3 Favg   = params.specularWeight * ( params.f0 + ( 1. - params.f0 ) / 21. );
        vec3 FmsEms = Ems * FssEss * Favg / ( 1.0 - Favg * Ems );
        layers.f_diffuse     = params.diffusebase * ( 1.0 - FssEss + FmsEms ) + FmsEms;

#ifdef SHEEN_LAYER
        r = params.sheen.sheenColorRough.a;
        layers.f_sheen = params.sheen.sheenColorRough.rgb * texture(material.sheen.charlieLUT, vec2( cosTo, r ) ).b;
        layers.sheen_scaling = 1.0 - max3(params.sheen.sheenColorRough.rgb) * albedoSheenScalingLUT(material.sheen, cosTo, r);
#endif

#ifdef CLEARCOAT_LAYER
        cosTo = clamp( dot(wo, params.clearcoat.normal), 0.0, 1.0 );
        r = params.clearcoat.intrough.y;
        f_ab = texture( material.ggxLut, vec2( cosTo, r ) ).rg;
        Fr     = max( vec3( 1. - r ), params.clearcoat.f0 ) - params.clearcoat.f0;
        Ks     = params.clearcoat.f0 + Fr * pow( 1. - cosTo, 5.0 );
        layers.f_clearcoat =  Ks * f_ab.x + f_ab.y;
#endif
        result = 1;
    }
    else
    {
        layers.f_diffuse  = vec3( 0 );
        layers.f_specular = vec3( 0 );
        result = 0;
    }

    return result;
}

#endif // GLTF_BASEMATERIAL_GLSL
