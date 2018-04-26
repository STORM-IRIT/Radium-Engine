#include "Structs.glsl"

uniform Material material;
uniform Light light;
uniform sampler2D uShadowMap;

out vec4 fragColor;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_texcoord;
layout (location = 3) in vec3 in_eye;
layout (location = 4) in vec3 in_tangent;

#include "LightingFunctions.glsl"

#if 0
float InShadow()
{
    vec3 proj_coords = in_position_light_space.xyz / in_position_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest = texture(uShadowMap, proj_coords.xy).r;
    float current = proj_coords.z;

    float result = current > closest ? 1.0 : 0.0;
    return result;
}
#endif

void main()
{
    if (toDiscard()) discard;

    //vec3 color = getKd() * 0.1 + (1 - Shadow()) * computeLighting();
    vec3 color = computeLighting();

    fragColor = vec4(color, 1.0);
}
