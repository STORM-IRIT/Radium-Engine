layout (location = 0) out vec4 fragSumColor;
layout (location = 1) out vec4 fragSumWeight;

struct Textures
{
    sampler2D kd;
    sampler2D alpha;

    int hasKd;
    int hasAlpha;
};

struct Material
{
    Textures tex;
    vec4 kd;
};

uniform Material material;
uniform float depthScale;

in vec3 vViewPosition;
in vec3 vTexcoord;
in float vDepth;

const float theDepth = 0.01;

vec3 getKd()
{
    if (material.tex.hasKd == 1)
    {
        return vec3(texture(material.tex.kd, vTexcoord.xy));
    }
    return material.kd.rgb;
}

float getAlpha()
{
    if (material.tex.hasAlpha == 1)
    {
        return texture(material.tex.alpha, vTexcoord.xy).r;
    }
    return material.kd.a;
}

void main()
{
    vec3 color = getKd();
    float a = getAlpha();
    float z = vDepth;
    
    if (a == 0)
    {
        discard;
    }
   
    float w = pow(a + 0.01f, 4.0f) +
                   max(0.01f, min(3000.0f, 0.3f / (0.00001f + pow(abs(z) / 200.0f, 4.0f))));
    
    fragSumColor = vec4(color* a, a) * w;
//    fragSumColor = vec4(vec3(w), 1.0);
    fragSumWeight = vec4(a);      
}

