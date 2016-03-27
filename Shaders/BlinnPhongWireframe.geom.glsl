layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
} gs_in[3];

out GS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 eye;
    vec3 triDistance;
} gs_out;

void main()
{
    vec3 triDistances[3];
    triDistances[0] = vec3(1, 0, 0);
    triDistances[1] = vec3(0, 1, 0);
    triDistances[2] = vec3(0, 0, 1);

    for (int i = 0; i < 3; ++i)
    {
        gs_out.normal       = gs_in[i].normal;
        gs_out.position     = gs_in[i].position;
        gs_out.eye          = gs_in[i].eye;
        gs_out.texcoord     = gs_in[i].texcoord;
        gs_out.triDistance  = triDistances[i];

        gl_Position         = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();

//    EmitEdge(v0, v1);
//    EmitEdge(v0, v2);
//    EmitEdge(v1, v2);
}
