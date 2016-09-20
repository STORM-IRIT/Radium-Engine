#include "Structs.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec3 inTexcoord;
// TODO(Charly): Add other inputs

uniform Transform transform;
uniform Material material;

uniform mat4 uLightSpace;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
    mat3 TBN;
    vec4 position_light_space;
} vs_out;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(inPosition, 1.0);

    vec4 pos = transform.model * vec4(inPosition, 1.0);
    pos /= pos.w;
    vec4 normal = transform.worldNormal * vec4(inNormal, 0.0);

    vec3 eye = -transform.view[3].xyz * mat3(transform.view);

    vs_out.position = vec3(pos);
    vs_out.normal   = vec3(normal);
    vs_out.eye      = vec3(eye);

    vs_out.texcoord = inTexcoord;

    if (material.tex.hasNormal == 1)
    {
        vec3 t = normalize(vec3(transform.model * vec4(inTangent,   0.0)));
        vec3 b = normalize(vec3(transform.model * vec4(inBitangent, 0.0)));
        vec3 n = normalize(vec3(transform.model * vec4(inNormal,    0.0)));

        vs_out.TBN = mat3(t, b, n);
    }

    mat4 light_proj;
    light_proj[0] = vec4(0.1, 0.0, 0.0, 0.0);
    light_proj[1] = vec4(0.0, 0.1, 0.0, 0.0);
    light_proj[2] = vec4(0.0, 0.0, -0.307692319, 0.0);
    light_proj[3] = vec4(0.0, 0.0, -1.30769229, 1.0);

    mat4 light_view;
    light_view[0] = vec4(-0.000000000, -0.995037258, 0.0995037258, 0.0);
    light_view[1] = vec4(0.000000000, 0.0995037258, 0.995037258, 0.0);
    light_view[2] = vec4(-1.00000000, 0.0, 0.0, 0.0);
    light_view[3] = vec4(0.0, 0.0, -10.0498762, 1.0);

    //vs_out.position_light_space = uLightSpace * vec4(vs_out.position, 1.0);
    //vs_out.position_light_space = light_proj * light_view * vec4(vs_out.position, 1.0);
}
