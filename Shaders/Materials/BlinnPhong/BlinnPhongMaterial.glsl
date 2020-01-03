#ifndef BLINNPHONGMATERIAL_GLSL
#define BLINNPHONGMATERIAL_GLSL
// Blinn-Phong specular and exponent :
// As this shader is built on Blinn-Phong NDF, that approximate Cook-Torrance,
// Specular parameters from the asset must be converted so that specular highlight look the same than with
// standard Phong BRDF (divided both Ks and Ns by Pi looks good).
// See http://www.thetenthplanet.de/archives/255 for intuitions
const float Pi = 3.141592653589793;

struct BlinnPhongTextures
{
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

struct Material
{
    vec4 kd;
    vec4 ks;

    float ns;
    float alpha;

    int hasPerVertexKd;
    int renderAsSplat;

    BlinnPhongTextures tex;
};

vec3 getKd(Material material, vec2 texCoord)
{
    if (material.hasPerVertexKd == 1)
    {
        return in_color.xyz;
    }
    if (material.tex.hasKd == 1)
    {
        return vec3(texture(material.tex.kd, texCoord));
    }

    return material.kd.xyz;
}

vec3 getKs(Material material, vec2 texCoord)
{
    if (material.tex.hasKs == 1)
    {
        return vec3(texture(material.tex.ks, texCoord));
    }

    return material.ks.xyz / Pi; // Phong specular to blinn-phong exponent
}

float getNs(Material material, vec2 texCoord)
{
    float ns = material.ns;
    if (material.tex.hasNs == 1)
    {
        ns = texture(material.tex.ns, texCoord).r;
    }

    return max(ns, 0.001)/Pi; // Phong exponent to blinn-phong exponent
}

vec3 getNormal(Material material, vec2 texCoord, vec3 N, vec3 T, vec3 B)
{
    if (material.tex.hasNormal == 1) {
        mat3 tbn;

        tbn[0]  = T;
        tbn[1]  = B;
        tbn[2]  = N;

        vec3 normalLocal = normalize(vec3(texture(material.tex.normal, texCoord)) * 2 - 1);
        return normalize(tbn * normalLocal);
    }

    return normalize(N);
}

bool toDiscard(Material material, vec2 texCoord)
{
    if (material.tex.hasAlpha == 1)
    {
        float alpha = texture(material.tex.alpha, texCoord).r;
        if (alpha < 0.1)
        {
            return true;
        }
    }
    if ( material.renderAsSplat == 1)
    {
        return dot(texCoord, texCoord) > 1;
    }
    return false;
}

vec3 getDiffuseColor(Material material, vec2 texC) {
    return getKd(material, texC);
}

vec3 getSpecularColor(Material material, vec2 texC) {
    return getKs(material, texC);
}

vec3 computeMaterialInternal(Material material, vec2 texC, vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y) {
    vec3 H =  normalize(L + V);
    // http://www.thetenthplanet.de/archives/255
    float Ns = getNs(material, texC);
    vec3 Kd = getKd(material, texC) / Pi;

    // use the correct normalization factor for Blinn-Phong BRDF;
    float normalization = (Ns + 1) / (8 * Pi * clamp(pow(dot(L, H), 3.), 0.000001, 1.));
    vec3 Ks = getKs(material, texC) * normalization;

    vec3 diff = Kd;
    vec3 spec = pow(max(dot(N, H), 0.0), Ns) * Ks;

    return (diff + spec) * max(dot(L,N), 0.0) ;
}


uniform Material material;

#endif//BLINNPHONGMATERIAL_GLSL
