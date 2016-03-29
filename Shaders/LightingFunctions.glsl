#include "Helpers.glsl"

vec3 blinnPhongInternal(vec3 d, vec3 n)
{
    vec3 direction = normalize(d);
    vec3 normal = normalize(n);

    float diffFactor = max(dot(normal, -direction), 0.0);
    vec3 diff = diffFactor * light.color.xyz * getKd();

    vec3 viewDir = normalize(fs_in.position - fs_in.eye);
    vec3 halfVec = normalize(viewDir + direction);

    float specFactor = pow(max(dot(normal, -halfVec), 0.0), getNs());
    vec3 spec = specFactor * light.color.xyz * getKs();

    return diff + spec;
}

vec3 blinnPhongSpot()
{
    vec3 dir = -light.spot.direction;
    vec3 color;

    float d = length(dir);
    float attenuation = light.spot.attenuation.constant +
                        light.spot.attenuation.linear * d +
                        light.spot.attenuation.quadratic * d * d;
    attenuation = 1.0 / attenuation;

    vec3 lightToPixel = normalize(fs_in.position - light.spot.position);

    float cosRealAngle = dot(lightToPixel, normalize(dir));
    float cosSpotOuter = cos(light.spot.innerAngle / 2.0);

    float radialAttenuation = pow(clamp((cosRealAngle - cosSpotOuter) /
                                        (1.0 - cosSpotOuter), 0.0, 1.0), 1.6);

    color = blinnPhongInternal(dir, getNormal());
    return color * attenuation * radialAttenuation;
}

vec3 blinnPhongPoint()
{
    vec3 dir = fs_in.position - light.point.position;
    vec3 color = blinnPhongInternal(normalize(dir), getNormal());

    float d = length(dir);
    float attenuation = light.point.attenuation.constant +
                        light.point.attenuation.linear * d +
                        light.point.attenuation.quadratic * d * d;

    return color / attenuation;
}

vec3 blinnPhongDirectional()
{
    return blinnPhongInternal(light.directional.direction, getNormal()) + getKd() * 0.1;
}

vec3 computeLighting()
{
    switch (light.type)
    {
        case 0:  return blinnPhongDirectional();
        case 1:  return blinnPhongPoint();
        case 2:  return blinnPhongSpot();
        default: return vec3(0);
    }
}
