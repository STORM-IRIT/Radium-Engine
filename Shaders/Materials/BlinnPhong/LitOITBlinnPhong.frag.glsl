layout (location = 0) out vec4 f_Accumulation;
layout (location = 1) out vec4 f_Revealage;

#include "TransformStructs.glsl"
#include "DefaultLight.glsl"
#include "BlinnPhongMaterial.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_viewVector;
layout (location = 5) in vec3 in_lightVector;


void main()
{

    if (toDiscard(material, in_texcoord.xy) || material.alpha < 0.01)
    {
        discard;
    }

    float a             = material.alpha;
    float z             = -in_position.z;

    float va            = (a + 0.01f);
    float va2           = va * va;
    float va4           = va2 * va2; // Pow4

    float vz            = abs(z) / 200.0f;
    float vz2           = vz * vz;
    float vz4           = vz2 * vz2;

    float w             = va4 + clamp(0.3f / (0.00001f + vz4), 0.01, 3000.0);

    vec3 binormal       = normalize(cross(in_normal, in_tangent));
    vec3 normalLocal    = getNormal(material, in_texcoord.xy, in_normal, in_tangent, binormal);
    vec3 binormalLocal  = normalize(cross(normalLocal, in_tangent));
    vec3 tangentLocal   = normalize(cross(binormalLocal, normalLocal));

    vec3 materialColor  = computeMaterialInternal(material, in_texcoord.xy, in_lightVector, in_viewVector,
                                                  normalLocal, tangentLocal, binormalLocal);

    vec3 contribution   = lightContributionFrom(light, in_position);

    f_Accumulation      = vec4(materialColor * contribution * a, a) * w;
    f_Revealage         = vec4(a);
}
