layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 15) out;

uniform mat4 view;
uniform mat4 proj;

in vec3 varPosition[];
in vec3 varNormal[];
in vec3 varEye[];

in vec3 varPositionInCamera[];
in vec3 varNormalInCamera[];

out vec3 gPosition;
out vec3 gNormal;
out vec3 gEye;

flat out int gIsEdge;

float edgeWidth = 0.1;
float pctExtend = 0.1;

bool isFrontFacing(vec3 a, vec3 b, vec3 c)
{
    return dot(cross(b - a, c - a), varEye[0]) < 0;
}

bool isFrontFacing(int a, int b, int c)
{
    vec3 p0 = varPosition[a];
    vec3 p1 = varPosition[b];
    vec3 p2 = varPosition[c];

    return isFrontFacing(p0, p1, p2);
}

void emitEdgeQuad(vec3 e0, vec3 e1)
{
    vec2 ext = pctExtend * (e1.xy - e0.xy);
    vec2 v = normalize(e1.xy - e0.xy);
    vec2 n = vec2(-v.y, v.x) * edgeWidth;

    gIsEdge = 1;

    gl_Position = vec4(e0.xy - ext, e0.z, 1.0); EmitVertex();
    gl_Position = vec4(e0.xy - n - ext, e0.z, 1.0); EmitVertex();
    gl_Position = vec4(e1.xy + ext, e1.z, 1.0); EmitVertex();
    gl_Position = vec4(e1.xy - n + ext, e1.z, 1.0); EmitVertex();
    EndPrimitive();
}

void main()
{
    vec3 p0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    vec3 p1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    vec3 p2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    vec3 p3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    vec3 p4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
    vec3 p5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;

    if (isFrontFacing(p0, p2, p4))
    {
        emitEdgeQuad(p0, p2);
//        if (!isFrontFacing(p0, p1, p2)) { emitEdgeQuad(p0, p2); }
//        if (!isFrontFacing(p2, p3, p4)) { emitEdgeQuad(p2, p4); }
//        if (!isFrontFacing(p4, p5, p0)) { emitEdgeQuad(p4, p0); }
    }

    gIsEdge = 0;
    gNormal = varNormal[0];
    gPosition = varPosition[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gNormal = varNormal[2];
    gPosition = varPosition[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    gNormal = varNormal[4];
    gPosition = varPosition[4];
    gl_Position = gl_in[4].gl_Position;
    EmitVertex();
    EndPrimitive();
}
