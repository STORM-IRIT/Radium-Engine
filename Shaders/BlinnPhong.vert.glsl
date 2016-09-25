#include "Structs.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tangent;
layout (location = 3) in vec3 in_bitangent;
layout (location = 4) in vec3 in_texcoord;
// TODO(Charly): Add other inputs

uniform Transform transform;
uniform Material material;

uniform mat4 uLightSpace;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec3 out_texcoord;
layout (location = 3) out vec3 out_eye;
layout (location = 4) out vec3 out_tangent; 

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(in_position, 1.0);

    vec4 pos = transform.model * vec4(in_position, 1.0);
    pos /= pos.w;
    vec3 normal = mat3(transform.worldNormal) * in_normal;

    vec3 eye = -transform.view[3].xyz * mat3(transform.view);

    out_position = vec3(pos);
    out_normal   = normal;
    out_eye      = vec3(eye);
    out_tangent  = in_tangent;

    out_texcoord = in_texcoord;

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

    //out_position_light_space = uLightSpace * vec4(out_position, 1.0);
    //out_position_light_space = light_proj * light_view * vec4(out_position, 1.0);
}
