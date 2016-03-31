struct Textures
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

    Textures tex;
};

uniform Material material;

in vec3 vTexcoord;
in vec4 vColor;
out vec4 fragColor;

void main()
{
    if ( material.tex.hasKd == 1 )
    {
        vec4 color = texture( material.tex.kd, vTexcoord.xy ).rgba;
        if (material.tex.hasAlpha == 1)
        {
            float alpha = texture(material.tex.alpha, vTexcoord.xy).r;
            if (alpha < 0.5)
            {
                discard;
            }
        }

        fragColor = vec4( color.rgb, 1.0 );
    }
    else
    {
        fragColor = vColor;
    }
}

