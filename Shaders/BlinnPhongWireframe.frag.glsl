#include "Structs.glsl"

uniform Material material;
uniform Light light;

uniform int plainRendering;

out vec4 fragColor;

in GS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
    vec3 triDistance;
} fs_in;

#include "LightingFunctions.glsl"

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2 * d * d);
    return d;
}

void main()
{
    if (toDiscard()) discard;

    vec3 color = computeLighting();

    float d1 = min(min(fs_in.triDistance.x, fs_in.triDistance.y), fs_in.triDistance.z);
    // NOTE(Charly): Play with the params
    float d = amplify(d1, 90, 0.0);

    color = d * color;

    fragColor = vec4(color, 1.0);
}
