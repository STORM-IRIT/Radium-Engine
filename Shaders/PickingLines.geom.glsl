#include "Structs.glsl"

// FIXME (florian): TO BE TESTED

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;

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
uniform float lineWidth;

layout (location = 0) out vec3  out_position;
layout (location = 1) out vec3  out_normal;
layout (location = 2) out vec3  out_eye;
layout (location = 3) out int   out_eltID;
layout (location = 4) out vec3  out_eltCoords;

void main()
{
    vec3 p0 = in_position[0];
    vec3 p1 = in_position[1];

    vec3 line = p1 - p0;
    vec3 n = normalize(vec3(-line.y, line.x, 0));

    vec3 a = p0 + n;
    vec3 b = p0 - n;
    vec3 c = p1 + n;
    vec3 d = p1 - n;

    gl_Position   = transform.proj * transform.view * vec4(a,1);
    out_position  = a;
    out_normal    = in_normal[0];
    out_eye       = in_eye[0];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(1,0,0);
    EmitVertex();

    gl_Position   = transform.proj * transform.view * vec4(b,1);
    out_position  = b;
    out_normal    = in_normal[0];
    out_eye       = in_eye[0];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(1,0,0);
    EmitVertex();

    gl_Position   = transform.proj * transform.view * vec4(c,1);
    out_position  = c;
    out_normal    = in_normal[1];
    out_eye       = in_eye[1];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(0,1,0);
    EmitVertex();

    EndPrimitive();

    gl_Position   = transform.proj * transform.view * vec4(b,1);
    out_position  = b;
    out_normal    = in_normal[0];
    out_eye       = in_eye[0];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(1,0,0);
    EmitVertex();

    gl_Position   = transform.proj * transform.view * vec4(c,1);
    out_position  = c;
    out_normal    = in_normal[1];
    out_eye       = in_eye[1];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(0,1,0);
    EmitVertex();

    gl_Position   = transform.proj * transform.view * vec4(d,1);
    out_position  = d;
    out_normal    = in_normal[1];
    out_eye       = in_eye[1];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(0,1,0);
    EmitVertex();

    EndPrimitive();
}
