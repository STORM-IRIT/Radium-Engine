#include "DefaultLight.glsl"
#include "Lambertian.glsl"
#include "VertexAttribInterface.frag.glsl"

layout( location = 6 ) in vec3 in_lightVector;

out vec4 out_color;

void main() {
    vec4 bc            = getDiffuseColor( material, getPerVertexTexCoord() );
    vec3 normalWorld   = getWorldSpaceNormal();
    vec3 tangentWorld  = getWorldSpaceTangent();   // normalized tangent
    vec3 binormalWorld = getWorldSpaceBiTangent(); // normalized bitangent

    // Computing attributes using dfdx or dfdy must be done before discarding fragments
    if ( toDiscard( material, bc ) ) discard;

    vec3 lightDir = normalize( in_lightVector ); // incident direction
    normalWorld   = getNormal( material,
                               getPerVertexTexCoord(),
                               normalWorld,
                               tangentWorld,
                               binormalWorld ); // normalized bump-mapped normal

    vec3 le   = lightContributionFrom( light, getWorldSpacePosition().xyz );
    out_color = vec4( (bc.rgb / Pi) * dot( normalWorld, lightDir ) * le, 1 );
}
