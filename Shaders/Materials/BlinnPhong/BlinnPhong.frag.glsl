// This is for a preview of the shader composition, but in time we must use more specific Light
// Shader
#include "DefaultLight.glsl"

#include "BlinnPhong.glsl"

#include "VertexAttribInterface.frag.glsl"

layout( location = 5 ) in vec3 in_viewVector;
layout( location = 6 ) in vec3 in_lightVector;

out vec4 fragColor;

// -----------------------------------------------------------

void main() {
    // discard non opaque fragment
    vec4 bc = getDiffuseColor( material, getPerVertexTexCoord() );
    if ( toDiscard( material, bc ) ) discard;
    // All vectors are in world space
    // A material is always evaluated in the fragment local Frame
    // compute matrix from World to local Frame
    vec3 normalWorld   = getWorldSpaceNormal();    // normalized interpolated normal
    vec3 tangentWorld  = getWorldSpaceTangent();   // normalized tangent
    vec3 binormalWorld = getWorldSpaceBiTangent(); // normalized bitangent
    // Apply normal mapping
    normalWorld   = getNormal( material,
                             getPerVertexTexCoord(),
                             normalWorld,
                             tangentWorld,
                             binormalWorld ); // normalized bump-mapped normal
    binormalWorld = normalize( cross( normalWorld, tangentWorld ) );  // normalized tangent
    tangentWorld  = normalize( cross( binormalWorld, normalWorld ) ); // normalized bitangent

    mat3 world2local;
    world2local[0] = vec3( tangentWorld.x, binormalWorld.x, normalWorld.x );
    world2local[1] = vec3( tangentWorld.y, binormalWorld.y, normalWorld.y );
    world2local[2] = vec3( tangentWorld.z, binormalWorld.z, normalWorld.z );
    // transform all vectors in local frame so that N = (0, 0, 1);
    vec3 lightDir = normalize( world2local * in_lightVector ); // incident direction
    vec3 viewDir  = normalize( world2local * in_viewVector );  // outgoing direction

    vec3 bsdf = evaluateBSDF( material, getPerVertexTexCoord(), lightDir, viewDir );

    vec3 contribution = lightContributionFrom( light, getWorldSpacePosition().xyz );
    fragColor         = vec4( bsdf * contribution, 1.0 );
}
