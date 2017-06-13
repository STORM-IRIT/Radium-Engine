/* RADIUM_SHADER_GLOBAL_REPLACE_VERTEX */

#include "Structs.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 4) in vec3 in_texcoord;
layout (location = 5) in vec4 in_color;

uniform Transform transform;
uniform int drawFixedSize;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_texcoord;
layout (location = 2) out vec3 out_color;

void main()
{
    mat4 mvp;
    if ( drawFixedSize > 0 )
    {
        // distance to camera
        mat4 modelView = transform.view * transform.model;
        float d = length( modelView[3].xyz );
        mat3 scale3 = mat3(d);
        mat4 scale = mat4(scale3);
        mat4 model = transform.model * scale;
        mvp = transform.proj * transform.view * model;
    }
    else
    {
        mvp = transform.proj * transform.view * transform.model;
    }

    gl_Position = mvp * vec4(in_position.xyz, 1.0);
    out_color = in_color.xyz;
    out_texcoord = in_texcoord;

    vec4 pos = transform.model * vec4(in_position, 1.0);
    pos /= pos.w;
    out_position = vec3(pos);
}
