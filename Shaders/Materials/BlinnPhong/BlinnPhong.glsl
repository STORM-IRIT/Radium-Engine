#ifndef BLINNPHONGMATERIAL_GLSL
#define BLINNPHONGMATERIAL_GLSL

//------------------- VertexAttrib interface ---------------------
vec4 getPerVertexBaseColor();
//----------------------------------------------------------------

// Blinn-Phong specular and exponent :
// As this shader is built on Blinn-Phong NDF, that approximate Cook-Torrance,
// Specular parameters from the asset must be converted so that specular highlight look the same
// than with standard Phong BRDF (divided both Ks and Ns by Pi looks good). See
// http://www.thetenthplanet.de/archives/255 for intuitions
const float Pi         = 3.141592653589793;
const float OneOver2Pi = 0.159154943091895;

struct BlinnPhongTextures {
    int hasKd;
    int hasKs;
    int hasNs;
    int hasNormal;
    int hasAlpha;

    sampler2D kd;
    sampler2D ks;
    sampler2D ns;
    sampler2D normal;
    sampler2D alpha;
};

struct Material {
    vec4 kd;
    vec4 ks;

    float ns;
    float alpha;

    int hasPerVertexKd;
    int renderAsSplat;

    BlinnPhongTextures tex;
};

// Du to access to in_vertexColor this does not respect the "Material.glsl" interface as it access
// data outside the material
vec4 getDiffuseColor( Material material, vec3 texCoord ) {
    vec4 dc = vec4( material.kd.rgb, material.alpha );
    if ( material.hasPerVertexKd == 1 ) { dc.rgb = getPerVertexBaseColor().rgb; }
    if ( material.tex.hasKd == 1 ) { dc.rgb = texture( material.tex.kd, texCoord.xy ).rgb; }
    if ( material.tex.hasAlpha == 1 ) { dc.a *= texture( material.tex.alpha, texCoord.xy ).r; }
    if ( material.renderAsSplat == 1 ) { dc.a = ( dot( texCoord.xy, texCoord.xy ) > 1 ) ? 0 : 1; }
    return dc;
}

vec4 getBaseColor( Material material, vec3 texCoord ) {
    return getDiffuseColor( material, texCoord );
}

vec3 getSpecularColor( Material material, vec3 texCoord ) {
    if ( material.tex.hasKs == 1 ) { return vec3( texture( material.tex.ks, texCoord.xy ) ); }

    return material.ks.xyz / Pi; // Phong specular to blinn-phong exponent
}

float getNs( Material material, vec2 texCoord ) {
    float ns = material.ns;
    if ( material.tex.hasNs == 1 ) { ns = texture( material.tex.ns, texCoord ).r; }

    return max( ns, 0.001 ) / Pi; // Phong exponent to blinn-phong exponent
}

vec3 getNormal( Material material, vec3 texCoord, vec3 N, vec3 T, vec3 B ) {
    if ( material.tex.hasNormal == 1 )
    {
        vec3 normalLocal = normalize( vec3( texture( material.tex.normal, texCoord.xy ) ) * 2 - 1 );
        mat3 tbn;
        tbn[0] = T;
        tbn[1] = B;
        tbn[2] = N;
        return normalize( tbn * normalLocal );
    }

    return normalize( N );
}

bool toDiscard( Material material, vec4 color ) {
    return color.a < 1;
}

// 139 ou 236

// wi (light direction) and wo (view direction) are in local frame
// wi dot N is then wi.z ...

vec3 evaluateBSDF( Material material, vec3 texC, vec3 l, vec3 v ) {
    // compute half vector
    vec3 h = normalize( l + v );
    // http://www.thetenthplanet.de/archives/255
    // Minimalist Cook-Torrance using Blinn-Phong approximation
    vec3 Kd   = getDiffuseColor( material, texC ).rgb / Pi;
    vec3 diff = Kd;

    vec3 Ks  = getSpecularColor( material, texC );
    float Ns = getNs( material, texC.xy );

    float D   = ( Ns + 1 ) * OneOver2Pi * pow( max( h.z, 0.0 ), Ns );
    float FV  = 0.25 * pow( dot( l, h ), -3. );
    vec3 spec = Ks * D * FV;
    return ( diff + spec ) * max( l.z, 0.0 );
}

uniform Material material;

#endif // BLINNPHONGMATERIAL_GLSL
