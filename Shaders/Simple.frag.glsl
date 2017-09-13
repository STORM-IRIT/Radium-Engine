#include "Structs.glsl"


layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_eye;

in vec2 uv_final;

uniform Transform transform;
uniform Material material;
vec2 in_texcoord;
vec3 in_tangent;

#include "Helpers.glsl"

layout (location = 0) out vec4 out_position;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_eye;
layout (location = 3) out vec3 out_ks;
layout (location = 4) out vec3 out_kd;
layout (location = 5) out float out_ns;

void main()
{
    if(length(uv_final) > 1.0 ) discard;
    out_normal = vec4(in_normal, 1.0);
    out_position = vec4(in_position, 1.0);
    in_texcoord = (in_position.xy+1.0)/2.0;;
    out_eye = vec4(in_eye, 1.0);
    out_ks = getKs();
    out_kd = getKd();
    out_ns = getNs();
}
