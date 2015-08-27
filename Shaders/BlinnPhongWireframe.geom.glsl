layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vPosition[3];
in vec3 vNormal[3];
in vec3 vTexcoord[3];
in vec3 vEye[3];

out vec3 gPosition;
out vec3 gNormal;
out vec3 gTexcoord;
out vec3 gEye;
out vec3 gTriDistance;

void main()
{
    vec3 triDistances[3];
    triDistances[0] = vec3(1, 0, 0);
    triDistances[1] = vec3(0, 1, 0);
    triDistances[2] = vec3(0, 0, 1);

    for (int i = 0; i < 3; ++i)
    {
        gNormal = vNormal[i];
        gPosition = vPosition[i];
        gEye = vEye[i];
        gTexcoord = vTexcoord[i];
        gTriDistance = triDistances[i];
        gl_Position = gl_in[i].gl_Position; EmitVertex();
    }

    EndPrimitive();

//    EmitEdge(v0, v1);
//    EmitEdge(v0, v2);
//    EmitEdge(v1, v2);
}
