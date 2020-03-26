struct Transform {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 mvp;
    mat4 modelView;
    mat4 worldNormal;
    mat4 viewNormal;
};

struct Textures {
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

/*
// Not sure that this must be here.
// Material definition "a la Disney BSDF"
// Correspondances between default material and others material are in comments
// see http://www.pbrt.org/fileformat-v3.html#materials
struct Material
{
    vec4 kd; // Diney : color

}
*/

struct Material {
    vec4 kd;
    vec4 ks;

    float ns;
    float alpha;

    Textures tex;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
};

struct PointLight {
    vec3 position;
    Attenuation attenuation;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    Attenuation attenuation;

    float innerAngle;
    float outerAngle;
};

struct Light {
    int type;
    vec4 color;

    DirectionalLight directional;
    PointLight point;
    SpotLight spot;
};
