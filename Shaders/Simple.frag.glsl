#include "Structs.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

layout (location = 0) out vec4 out_position;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 secondary_position;
layout (location = 4) in vec2 in_uv;

uniform Transform transform;

void main()
{
    if(length(in_uv) > 1.0 || in_normal.z < 0) discard;
    out_normal = vec4(in_normal, 1.0);
    out_position = vec4(in_position, 1.0);
    secondary_position = out_position;
}
