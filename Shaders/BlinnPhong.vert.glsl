#include "Structs.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec3 inTexcoord;
// TODO(Charly): Add other inputs

uniform Transform transform;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
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
}
