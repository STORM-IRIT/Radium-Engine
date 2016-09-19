layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec3 inTexcoord;

#include "Structs.glsl"

uniform Transform transform;
uniform Material material;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
	vec3 eye;
    mat3 TBN;
} vs_out;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(inPosition, 1.0);

    vec4 pos = transform.model * vec4(inPosition, 1.0);
    vs_out.position = pos.xyz / pos.w;
    vs_out.normal = vec3(transform.worldNormal * vec4(inNormal, 0.0));

    vs_out.texcoord = inTexcoord;

    if (material.tex.hasNormal == 1)
    {
        vec3 t = normalize(vec3(transform.model * vec4(inTangent,   0.0)));
        vec3 b = normalize(vec3(transform.model * vec4(inBitangent, 0.0)));
        vec3 n = normalize(vec3(transform.model * vec4(inNormal,    0.0)));

        vs_out.TBN = mat3(t, b, n);
    }
}
