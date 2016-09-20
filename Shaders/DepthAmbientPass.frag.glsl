layout (location = 0) out vec4 fragNormal;
layout (location = 1) out vec4 fragAmbient;

#include "Structs.glsl"

uniform Material material;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
    mat3 TBN;
} fs_in;

#include "Helpers.glsl"

void main()
{
    if (toDiscard()) discard;

    fragNormal = vec4( getNormal() * 0.5 + 0.5, 1.0 );
    fragAmbient = vec4(getKd() * 0.1, 1.0);
}
