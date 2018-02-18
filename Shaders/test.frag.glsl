#include "Structs.glsl"

uniform Material material;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
layout (location = 2) in vec3 in_color;
layout (location = 3) in vec3 in_triangleCoords;
layout (location = 4) flat in int in_coloredEdges;

out vec4 out_color;

void main()
{
    float eps = 0.05;
    if ( (in_triangleCoords[0] < eps && (in_coloredEdges & 1) != 0) ||
         (in_triangleCoords[1] < eps && (in_coloredEdges & 2) != 0) ||
         (in_triangleCoords[2] < eps && (in_coloredEdges & 4) != 0) )
    {
        out_color = vec4(in_color.rgb, 1.0);
    }
    else
    {
        out_color = vec4(0., 0., 0., 1.0);
    }


}
