layout (location = 0) out vec4 f_Accumulation;
layout (location = 1) out vec4 f_Revealage;

#include "TransformStructs.glsl"
#include "DefaultLight.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_viewVector;
layout (location = 5) in vec3 in_lightVector;
layout (location = 6) in vec3 in_color;

#include "BlinnPhongMaterial.glsl"


float weight(float z, float alpha) {

     // pow(alpha, colorResistance) : increase colorResistance if foreground transparent are affecting background transparent color
     // clamp(adjust / f(z), min, max) :
     //     adjust : Range adjustment to avoid saturating at the clamp bounds
     //     clamp bounds : to be tuned to avoid over or underflow of the reveleage texture.
     // f(z) = 1e-5 + pow(z/depthRange, orederingStrength)
     //     defRange : Depth range over which significant ordering discrimination is required. Here, 10 camera space units.
     //         Decrease if high-opacity surfaces seem “too transparent”,
     //         increase if distant transparents are blending together too much.
     //     orderingStrength : Ordering strength. Increase if background is showing through foreground too much.
     // 1e-5 + ... : avoid dividing by zero !

     return pow(alpha, 0.5) * clamp(10 / ( 1e-5 + pow(z/10, 6)  ), 1e-2, 3*1e3);
 }

void main()
{

    // compute the transparency factor
    float a             = material.alpha;
    // discard fully transparent fragment
    if (toDiscard(material, in_texcoord.xy) || a < 0.01)
    {
        discard;
    }

    vec3 binormal       = normalize(cross(in_normal, in_tangent));
    vec3 normalLocal    = getNormal(material, in_texcoord.xy, in_normal, in_tangent, binormal);
    vec3 binormalLocal  = normalize(cross(normalLocal, in_tangent));
    vec3 tangentLocal   = normalize(cross(binormalLocal, normalLocal));

    vec3 materialColor  = computeMaterialInternal(material, in_texcoord.xy, in_lightVector, in_viewVector,
                                                  normalLocal, tangentLocal, binormalLocal);
    vec3 contribution   = lightContributionFrom(light, in_position);

    float w             = weight(gl_FragCoord.z, a);
    f_Accumulation     = vec4(materialColor * contribution * a , a) * w;
    f_Revealage        = vec4(a);

}
