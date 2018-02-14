#include "Structs.glsl"

layout(points) in;
layout(points, max_vertices = 1) out;

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

layout (location = 0) in vec3 in_position[];
layout (location = 1) in vec3 in_normal[];
layout (location = 2) in vec3 in_eye[];

uniform Transform transform;

layout (location = 0)      out vec3  out_position;
layout (location = 1)      out vec3  out_normal;
layout (location = 2)      out vec3  out_eye;
layout (location = 3) flat out int   out_eltID;
layout (location = 4)      out vec3  out_eltCoords;

void main()
{
    gl_Position   = transform.proj * transform.view * vec4(in_position[0],1);
    out_position  = in_position[0];
    out_normal    = in_normal[0];
    out_eye       = in_eye[0];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3(1,0,0);
    EmitVertex();
    EndPrimitive();
}
