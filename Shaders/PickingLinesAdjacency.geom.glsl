#include "Structs.glsl"

layout(lines_adjacency) in;
layout(line_strip, max_vertices = 2) out;

in gl_PerVertex {
    vec4  gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];

out gl_PerVertex {
    vec4  gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

layout (location = 0) in vec3 in_position[4];
layout (location = 1) in vec3 in_normal[4];
layout (location = 2) in vec3 in_eye[4];

uniform Transform transform;

layout (location = 0)      out vec3  out_position;
layout (location = 1)      out vec3  out_normal;
layout (location = 2)      out vec3  out_eye;
layout (location = 3) flat out int   out_eltID;
layout (location = 4)      out vec3  out_eltCoords;

void main()
{
    vec3 p1 = in_position[1];
    vec3 p2 = in_position[2];

    gl_Position   = transform.proj * transform.view * vec4(p1,1);
    out_position  = p1;
    out_normal    = in_normal[1];
    out_eye       = in_eye[1];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(1,0,0);
    EmitVertex();

    gl_Position   = transform.proj * transform.view * vec4(p2,1);
    out_position  = p2;
    out_normal    = in_normal[2];
    out_eye       = in_eye[2];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(0,1,0);
    EmitVertex();

    EndPrimitive();
}
