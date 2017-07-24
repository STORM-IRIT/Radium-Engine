#include "Structs.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

uniform Transform transform;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;

out vec3 transformed_position;
out vec3 transformed_normal;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(in_position, 1.0);

    vec4 pos =  mvp * vec4(in_position, 1.0);
    pos /= pos.w;
    vec3 normal =mat3(transform.proj) * mat3(transform.view) * mat3(transform.worldNormal) * in_normal;//mat3(transform.proj) * mat3(transform.viewNormal) * mat3(transform.worldNormal) * in_normal;

    transformed_position = pos.xyz;
    transformed_normal   = normal;
}
