#include "TransformStructs.glsl"
#include "DefaultLight.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tangent;
layout (location = 3) in vec3 in_bitangent;
layout (location = 4) in vec3 in_texcoord;
layout (location = 5) in vec4 in_color;

uniform Transform transform;

layout (location = 0) out vec3 out_position;// Position in World Space
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec3 out_texcoord;// Used in case of normal mapping
layout (location = 3) out vec3 out_vertexcolor;
layout (location = 4) out vec3 out_tangent;
layout (location = 5) out vec3 out_viewVector;
layout (location = 6) out vec3 out_lightVector;


void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(in_position, 1.0);

    vec4 pos = transform.model * vec4(in_position, 1.0);
    pos /= pos.w;

    vec3 eye = -transform.view[3].xyz * mat3(transform.view);

    out_position    = vec3(pos);
    out_texcoord    = in_texcoord;
    out_normal      = normalize(mat3(transform.worldNormal) * in_normal);
    out_tangent     = normalize(mat3(transform.model) * in_tangent);
    out_viewVector  = normalize(eye - pos.xyz);
    out_lightVector = getLightDirection(light, pos.xyz);
    out_vertexcolor = in_color.rgb;
}
