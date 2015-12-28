layout (location = 0) out vec4 fragNormal;

struct Textures
{
    int hasNormal;
    int hasAlpha;

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

void main()
{
    if (material.tex.hasAlpha == 1)
    {
        float alpha = texture(material.tex.alpha, vTexcoord.xy).r;
        if (alpha < 0.1)
        {
            discard;
        }
    }

    fragNormal = vec4(getNormal() * 0.5 + 0.5, 1.0);
}
