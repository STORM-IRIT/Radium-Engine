layout (location = 0) out vec4 fragAmbient;
layout (location = 1) out vec4 fragPosition;
layout (location = 2) out vec4 fragNormal;

struct Textures
{
    int hasKd;
    int hasKs;
    int hasNormal;
    int hasAlpha;

    sampler2D kd;
    sampler2D ks;
    sampler2D normal;
    sampler2D alpha;
};

struct Material
{
    vec4 kd;
    vec4 ks;

    Textures tex;
};

uniform Material material;

in vec3 vPosition;
in vec3 vTexcoord;
in vec3 vNormal;

vec4 getKd()
{
    if (material.tex.hasKd == 1)
    {
        return vec4(texture(material.tex.kd, vTexcoord.xy));
    }
    else
    {
        return material.kd;
    }
}

vec4 getKs()
{
    if (material.tex.hasKs == 1)
    {
        return vec4(texture(material.tex.ks, vTexcoord.xy));
    }
    else
    {
        return material.ks;
    }
}

vec3 getNormal()
{
    float dir = gl_FrontFacing ? 1.0 : -1.0;
    if (material.tex.hasNormal == 1)
    {
        vec3 n = normalize(vec3(texture(material.tex.normal, vTexcoord.xy)));
        n = n * 2 - 1;
        return dir * n;
    }
    else
    {
        return vec3(dir * normalize(vNormal));
    }
}


void main()
{
    // FIXME(Charly): Ambient color "power" ?
    fragAmbient = vec4(getKd().xyz * 0.1, 1);
    fragPosition = vec4(vPosition, 1.0);
    fragNormal = vec4(getNormal(), 1.0);
}
