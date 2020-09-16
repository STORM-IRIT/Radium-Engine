#include "DefaultLight.glsl"
#include "Lambertian.glsl"
#include "VertexAttribInterface.frag.glsl"

layout( location = 5 ) in vec3 in_viewVector;
layout( location = 6 ) in vec3 in_lightVector;

out vec4 out_color;

void main() {
    vec4 bc     = getBaseColor( material, getPerVertexTexCoord() );
    vec3 normal = getWorldSpaceNormal();
    // Computing attributes using dfdx or dfdy must be done before discarding fragments
    if ( toDiscard( material, bc ) ) discard;

    vec3 le   = lightContributionFrom( light, getWorldSpacePosition().xyz );
    out_color = vec4( bc.rgb * dot( normal, normalize( in_lightVector ) ) * le, 1 );
}
