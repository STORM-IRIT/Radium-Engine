#include "Structs.glsl"

layout(lines) in;
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

layout (location = 0) in vec3 in_position[2];
layout (location = 1) in vec3 in_normal[2];
layout (location = 2) in vec3 in_eye[2];

uniform Transform transform;

layout (location = 0)      out vec3  out_position;
layout (location = 1)      out vec3  out_normal;
layout (location = 2)      out vec3  out_eye;
layout (location = 3) flat out int   out_eltID;
layout (location = 4)      out vec3  out_eltCoords;

void main()
{
    vec3 p0 = in_position[0];
    vec3 p1 = in_position[1];

    gl_Position   = transform.proj * transform.view * vec4(p0,1);
    out_position  = p0;
    out_normal    = in_normal[0];
    out_eye       = in_eye[0];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(1,0,0);
    EmitVertex();

    gl_Position   = transform.proj * transform.view * vec4(p1,1);
    out_position  = p1;
    out_normal    = in_normal[1];
    out_eye       = in_eye[1];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(0,1,0);
    EmitVertex();

    EndPrimitive();
}
