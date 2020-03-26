layout( lines_adjacency ) in;
layout( triangle_strip, max_vertices = 4 ) out;

in vec3 vPosition[4];
in vec4 vColor[4];

out vec4 gColor;

uniform float lineWidth;

void main() {
    vec3 p0 = vPosition[1];
    vec3 p1 = vPosition[2];

    vec3 line = p1 - p0;
    vec3 n    = normalize( vec3( -line.y, line.x, 0 ) );

    vec3 a = p0 + n;
    vec3 b = p0 - n;
    vec3 c = p1 + n;
    vec3 d = p1 - n;

    gl_Position.w = 1.0;

    gColor          = vColor[1];
    gl_Position.xyz = a;
    EmitVertex();

    gColor          = vColor[1];
    gl_Position.xyz = b;
    EmitVertex();

    gColor          = vColor[2];
    gl_Position.xyz = c;
    EmitVertex();

    EndPrimitive();

    gColor          = vColor[1];
    gl_Position.xyz = b;
    EmitVertex();

    gColor          = vColor[2];
    gl_Position.xyz = c;
    EmitVertex();

    gColor          = vColor[2];
    gl_Position.xyz = d;
    EmitVertex();

    EndPrimitive();
}
