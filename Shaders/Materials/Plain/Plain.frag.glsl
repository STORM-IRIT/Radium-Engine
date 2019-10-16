#include "DefaultLight.glsl"
#include "Plain.glsl"
#include "VertexAttribInterface.frag.glsl"

out vec4 out_color;

void main()
{
    vec4 bc = getBaseColor(material, getPerVertexTexCoord().xy);
    if (toDiscard(material, bc))
    discard;

    if (material.shaded == 1) {
        vec3 le    = lightContributionFrom(light, getWorldSpacePosition().xyz);
        out_color = vec4(bc.rgb * dot(getWorldSpaceNormal(), normalize(in_lightVector)) * le, 1);
    } else {
        out_color = vec4(bc.rgb, 1);
    }
}

