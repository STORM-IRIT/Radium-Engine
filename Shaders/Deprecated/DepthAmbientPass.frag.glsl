layout (location = 0) out vec4 out_ambient;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_diffuse;
layout (location = 3) out vec4 out_specular;

#include "Structs.glsl"

uniform Material material;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_texcoord;
layout (location = 3) in vec3 in_eye;
layout (location = 4) in vec3 in_tangent;

#include "Helpers.glsl"

void main()
{
    if (toDiscard()) discard;

    out_ambient = vec4(getKd() * 0.1, 1.0);
    out_normal = vec4( getNormal() * 0.5 + 0.5, 1.0 );
    out_diffuse = vec4(getKd(), 1.0);
    out_specular = vec4(getKs(), 1.0);
}
