out vec4 fragNormal;

#include "Structs.glsl"

uniform Material material;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
} fs_in;

#include "Helpers.glsl"

void main()
{
    if (toDiscard()) discard;

    fragNormal = vec4( getNormal() * 0.5 + 0.5, 1.0 );
}
