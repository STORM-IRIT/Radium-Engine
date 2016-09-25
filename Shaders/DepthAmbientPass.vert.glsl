layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tangent;
layout (location = 3) in vec3 in_bitangent;
layout (location = 4) in vec3 in_texcoord;

#include "Structs.glsl"

uniform Transform transform;
uniform Material material;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec3 out_texcoord;
layout (location = 3) out vec3 out_eye;
layout (location = 4) out mat3 out_TBN;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(in_position, 1.0);

    vec4 pos = transform.model * vec4(in_position, 1.0);
    out_position = pos.xyz / pos.w;
    out_normal = vec3(transform.worldNormal * vec4(in_normal, 0.0));

    out_texcoord = in_texcoord;

    if (material.tex.hasNormal == 1)
    {
        vec3 t = normalize(vec3(transform.model * vec4(in_tangent,   0.0)));
        vec3 b = normalize(vec3(transform.model * vec4(in_bitangent, 0.0)));
        vec3 n = normalize(vec3(transform.model * vec4(in_normal,    0.0)));

        out_TBN = mat3(t, b, n);
    }
}
