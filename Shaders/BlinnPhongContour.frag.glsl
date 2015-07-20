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

in vec3 gPosition;
in vec3 gNormal;
in vec3 gEye;
flat in int gIsEdge;

vec3 blinnPhongInternal(vec3 d, vec3 n)
{
    vec3 direction = normalize(d);
    vec3 normal = normalize(n);
    float diffFactor = dot(normal, -direction);

    vec3 diff = vec3(0);
    vec3 spec = vec3(0);

    if (diffFactor > 0.0)
    {
        diff = diffFactor * light.color.xyz * material.kd.xyz;

        vec3 vertToEye = normalize(gEye - gPosition);
        vec3 lightReflect = normalize(reflect(direction, normal));
        float specFactor = dot(vertToEye, lightReflect);

        if (specFactor > 0.0)
        {
            specFactor = pow(specFactor, 64.0);
            spec = specFactor * light.color.xyz * material.ks.xyz;
        }
    }

    return diff + spec;
}

vec3 blinnPhongSpot()
{
    vec3 dir = light.spot.direction;
    vec3 color;

    float d = length(dir);
    float attenuation = light.spot.attenuation.constant +
                        light.spot.attenuation.linear * d +
                        light.spot.attenuation.quadratic * d * d;
    attenuation = 1.0 / attenuation;

    vec3 lightToPixel = normalize(gPosition - light.spot.position);

    float cosRealAngle = dot(lightToPixel, -normalize(dir));
    float cosSpotOuter = cos(light.spot.innerAngle / 2.0);

    float radialAttenuation = pow(clamp((cosRealAngle - cosSpotOuter) /
                                        (1.0 - cosSpotOuter), 0.0, 1.0), 1.6);

    color = blinnPhongInternal(-dir, gNormal);
    return color * attenuation * radialAttenuation;
}

vec3 blinnPhongPoint()
{
    vec3 dir = gPosition - light.point.position;
    vec3 color = blinnPhongInternal(normalize(dir), normalize(gNormal));

    float d = length(dir);
    float attenuation = light.point.attenuation.constant +
                        light.point.attenuation.linear * d +
                        light.point.attenuation.quadratic * d * d;

    return color / attenuation;
}

vec3 blinnPhongDirectional()
{
    return blinnPhongInternal(light.directional.direction, normalize(gNormal));
}

void main()
{
    vec3 color;
    bool isEdge = bool(gIsEdge);
    if (isEdge)
    {
        color = vec3(1, 0, 0);
    }
    else
    {
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
    }

    fragColor = vec4(color, 1.0);
}
