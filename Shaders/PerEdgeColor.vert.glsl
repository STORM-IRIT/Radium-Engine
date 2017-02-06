// cf. http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/
// cf. https://gist.github.com/paulhoux/9583347

layout (location = 0) in vec3 in_position;
layout (location = 4) in vec3 in_texcoord;

layout (location = 0) out vec3 out_position;
layout (location = 4) out vec3 out_texcoord;

#include "Structs.glsl"

uniform Transform transform;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(in_position, 1.0);

    out_texcoord = in_texcoord;
    out_position = gl_Position.xyz;
}
