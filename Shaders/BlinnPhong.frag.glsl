#include "Structs.glsl"

uniform Material material;
uniform Light light;

out vec4 fragColor;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
} fs_in;

#include "LightingFunctions.glsl"

void main()
{
    if (toDiscard()) discard;

    vec3 color = computeLighting();

    fragColor = vec4(color, 1.0);
}
