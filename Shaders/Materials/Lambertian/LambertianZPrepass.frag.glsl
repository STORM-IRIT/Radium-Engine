#include "DefaultLight.glsl"
#include "Lambertian.glsl"
#include "VertexAttribInterface.frag.glsl"

layout( location = 0 ) out vec4 out_ambient;
layout( location = 1 ) out vec4 out_normal;
layout( location = 2 ) out vec4 out_diffuse;

void main() {
    vec4 bc = getBaseColor( material, getPerVertexTexCoord() );
    if ( toDiscard( material, bc ) ) discard;

    out_ambient = vec4( bc.rgb * 0.01 + getEmissiveColor( material, getPerVertexTexCoord() ), 1.0 );
    out_normal  = vec4( getWorldSpaceNormal() * 0.5 + 0.5, 1.0 );
    out_diffuse = vec4( bc.rgb, 1.0 );
}
