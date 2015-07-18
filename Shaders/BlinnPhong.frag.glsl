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

struct Attenuation
{
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight
{
    vec3 direction;
};

struct PointLight
{
    vec3 position;
    Attenuation attenuation;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    Attenuation attenuation;

    float innerAngle;
    float outerAngle;
};

struct Light
{
    int type;
    vec4 color;

    DirectionalLight directional;
    PointLight point;
    SpotLight spot;
};

uniform Material material;
uniform Light light;

out vec4 fragColor;

in vec3 varPosition;
in vec3 varNormal;

vec3 blinnPhongPoint()
{
    return vec3(1, 0, 0);
}

vec3 blinnPhongSpot()
{
    return vec3(0, 1, 0);
}

vec3 blinnPhongDirectional()
{
    vec3 normal = normalize(varNormal);
    vec3 lightDir = -normalize(light.directional.direction);

    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;

    if (lambertian > 0.0)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(-varPosition);

        float specAngle = max(dot(reflectDir, viewDir), 0.0);
        specular = pow(specAngle, 32.0); // FIXME(Charly): Specular exponent as ks.w
    }

    return vec3(lambertian * light.color * material.kd +
                specular * light.color * material.ks);
}

void main()
{
    vec3 color;
    switch (light.type)
    {
        case 0:
        {
            color = blinnPhongDirectional();
        } break;

        case 1:
        {
            color = blinnPhongPoint();
        } break;

        case 2:
        {
            color = blinnPhongSpot();
        } break;

        default:
        {
            color = vec3(0, 0, 0);
        } break;
    }

    fragColor = vec4(color, 1.0);
}
