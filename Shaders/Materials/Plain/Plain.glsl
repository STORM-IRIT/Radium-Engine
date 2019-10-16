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

vec3 getDiffuseColor(Material material, vec2 texCoord)
{
    if (material.perVertexColor == 1)
    {
        return getPerVertexBaseColor().xyz;
    }
    if (material.tex.hasColor == 1)
    {
        return vec3(texture(material.tex.color, texCoord));
    }

    return material.color.rgb;
}

// basecolor is diffuseColor
vec4 getBaseColor(Material material, vec2 texCoord) {
    if (material.tex.hasMask == 1 && texture(material.tex.mask, texCoord).r < 0.1) {
        return vec4(getDiffuseColor(material, texCoord), 0);
    } else {
        return vec4(getDiffuseColor(material, texCoord), 1);
    }
}

// only discard based on texture
bool toDiscard(Material material, vec4 color) {
    return (color.a < 0.1);
}

// wi (light direction) and wo (view direction) are in local frame
// wi dot N is then wi.z ...
vec3 evaluateBSDF(Material material, vec2 texC, vec3 l, vec3 v) {
    return max(l.z, 0.0) * getDiffuseColor(material, texC) / Pi;
}

uniform Material material;
