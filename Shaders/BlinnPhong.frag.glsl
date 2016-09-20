#include "Structs.glsl"

uniform Material material;
uniform Light light;
uniform sampler2D uShadowMap;

out vec4 fragColor;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
    vec4 position_light_space;
    mat3 TBN;
} fs_in;

#include "LightingFunctions.glsl"

float Shadow()
{
    vec3 proj_coords = fs_in.position_light_space.xyz / fs_in.position_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest = texture(uShadowMap, proj_coords.xy).r;
    float current = proj_coords.z;

    float result = current > closest ? 1.0 : 0.0;
    return result;
}

void main()
{
    if (toDiscard()) discard;

    vec3 color = getKd() * 0.1 + (1 - Shadow()) * computeLighting();
    //vec3 color = vec3(1 - Shadow());

    fragColor = vec4(color, 1.0);
}
