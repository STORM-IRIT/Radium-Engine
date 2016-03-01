out vec4 fragNormal;

struct Textures
{
    int hasKd;
    int hasNormal;
    int hasAlpha;

    sampler2D kd;
    sampler2D normal;
    sampler2D alpha;
};

struct Material
{
    Textures tex;
};

uniform Material material;

in vec3 vTexcoord;
in vec3 vNormal;

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

float getAlpha()
{
    if (material.tex.hasAlpha == 1)
    {
        return texture(material.tex.alpha, vTexcoord.xy).r;
    }

    if ( material.tex.hasKd == 1 )
    {
        return texture( material.tex.kd, vTexcoord.xy ).a;
    }

    return 1.0;
}

void main()
{
    float alpha = getAlpha();

    if ( alpha < 0.5 ) discard;

    fragNormal = vec4( getNormal() * 0.5 + 0.5, 1.0 );
//    fragNormal = vec4( alpha, alpha, alpha, 1.0 );

//    fragNormal = vec4( vTexcoord.xy, 0, 1 );
}
