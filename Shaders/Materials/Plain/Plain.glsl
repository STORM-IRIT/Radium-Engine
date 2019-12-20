// Assume that plain rendering is based on very simple "material"
// This material could be used with or without lighting.
// When used with lighting, behave like a pure lambertian material.
// When used without lighting, render as a plain, flat color
struct PlainTextures
{
    int hasColor;
    int hasMask;

    sampler2D color;
    sampler2D mask;
};

struct Material
{
    vec4 color;
    PlainTextures tex;

    int perVertexColor;
    int shaded;
};


//------------------- VertexAttrib interface ---------------------
vec4 getPerVertexBaseColor();
//----------------------------------------------------------------
const float Pi = 3.141592653589793;

vec4 getDiffuseColor(Material material, vec2 texCoord)
{
    vec4 dc = vec4 (material.color.rgb, 1);

    if (material.perVertexColor == 1)
    {
        dc.rgb = getPerVertexBaseColor().rgb;
    }
    if (material.tex.hasColor == 1)
    {
        dc.rgb = texture(material.tex.color, texCoord).rgb;
    }

    if (material.tex.hasMask == 1 && texture(material.tex.mask, texCoord).r < 0.1) {
        dc.a = 0;
    }
    return dc;
}

// basecolor is diffuseColor
vec4 getBaseColor(Material material, vec2 texCoord) {
    return getDiffuseColor(material, texCoord);
}

// only discard based on texture
bool toDiscard(Material material, vec4 color) {
    return (color.a < 0.1);
}

// wi (light direction) and wo (view direction) are in local frame
// wi dot N is then wi.z ...
vec3 evaluateBSDF(Material material, vec2 texC, vec3 l, vec3 v) {
    return max(l.z, 0.0) * getDiffuseColor(material, texC).rgb / Pi;
}

uniform Material material;
