#include "DefaultLight.glsl"
#include "Lambertian.glsl"
#include "VertexAttribInterface.frag.glsl"

layout( location = 5 ) in vec3 in_viewVector;
layout( location = 6 ) in vec3 in_lightVector;

out vec4 out_color;

void main() {
    vec4 bc = getBaseColor( material, getPerVertexTexCoord() );
    if ( toDiscard( material, bc ) ) discard;

    vec3 le   = lightContributionFrom( light, getWorldSpacePosition().xyz );
    out_color = vec4( bc.rgb * dot( getWorldSpaceNormal(), normalize( in_lightVector ) ) * le, 1 );
}
