#include "Structs.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

uniform Transform transform;
uniform Material material;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec3 out_eye;

void main()
{
    vec3 normal = normalize(mat3(transform.view) * mat3(transform.worldNormal) * in_normal);
    mat4 mv = transform.view * transform.model;
    vec4 pos =  mv * vec4(in_position, 1.0);
    vec3 eye = -transform.view[3].xyz * mat3(transform.view);
    pos /= pos.w;

    //out_mat = material;
    //out_normal = vec3(material.ns);
    out_eye = eye;
    out_position = pos.xyz;
    out_normal = normal.xyz;
    gl_Position = transform.proj * pos;
}
