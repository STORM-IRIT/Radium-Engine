#include "Structs.glsl"

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};


layout (location = 0) in vec3 in_position[3];
layout (location = 1) in vec3 in_texcoord[3];
layout (location = 2) in vec3 in_color[3];

uniform Transform transform;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_texcoord;
layout (location = 2) out vec3 out_color;
layout (location = 3) out vec3 out_triangleCoords;
layout (location = 4) flat out int out_coloredEdges;

void main()
{
    vec3 p0 = in_position[0];
    vec3 p1 = in_position[1];
    vec3 p2 = in_position[2];

    int coloredEdges = 0;
    if ( (in_color[0].x != 0 ||
          in_color[0].y != 0 ||
          in_color[0].z != 0 ) &&
         (in_color[1].x != 0 ||
          in_color[1].y != 0 ||
          in_color[1].z != 0 ) )
    {
        coloredEdges += 4;
    }
    if ( (in_color[2].x != 0 ||
          in_color[2].y != 0 ||
          in_color[2].z != 0 ) &&
         (in_color[1].x != 0 ||
          in_color[1].y != 0 ||
          in_color[1].z != 0 ) )
    {
        coloredEdges += 1;
    }
    if ( (in_color[0].x != 0 ||
          in_color[0].y != 0 ||
          in_color[0].z != 0 ) &&
         (in_color[2].x != 0 ||
          in_color[2].y != 0 ||
          in_color[2].z != 0 ) )
    {
        coloredEdges += 2;
    }

    gl_Position = transform.proj * transform.view * vec4(p0, 1.0);
    gl_Position.w += 0.001;
    out_position = p0;
    out_texcoord = in_texcoord[0];
    out_color = in_color[0];
    out_triangleCoords = vec3(1,0,0);
    out_coloredEdges = coloredEdges;
    EmitVertex();

    gl_Position = transform.proj * transform.view * vec4(p1, 1.0);
    gl_Position.w += 0.001;
    out_position = p1;
    out_texcoord = in_texcoord[1];
    out_color = in_color[1];
    out_triangleCoords = vec3(0,1,0);
    out_coloredEdges = coloredEdges;
    EmitVertex();

    gl_Position = transform.proj * transform.view * vec4(p2, 1.0);
    gl_Position.w += 0.001;
    out_position = p2;
    out_texcoord = in_texcoord[2];
    out_color = in_color[2];
    out_triangleCoords = vec3(0,0,1);
    out_coloredEdges = coloredEdges;
    EmitVertex();

    EndPrimitive();
}
