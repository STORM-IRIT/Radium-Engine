layout( lines ) in;
layout( triangle_strip, max_vertices = 4 ) out;

in gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
}
gl_in[];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

in vec3 vPosition[2];
in vec4 vColor[2];

out vec4 gColor;

uniform float lineWidth;

void main() {
    vec3 p0 = vPosition[0];
    vec3 p1 = vPosition[1];

    vec3 line = p1 - p0;
    vec3 n    = normalize( vec3( -line.y, line.x, 0 ) );

    vec3 a = p0 + n;
    vec3 b = p0 - n;
    vec3 c = p1 + n;
    vec3 d = p1 - n;

    gl_Position.w = 1.0;

    gColor          = vColor[0];
    gl_Position.xyz = a;
    EmitVertex();

    gColor          = vColor[0];
    gl_Position.xyz = b;
    EmitVertex();

    gColor          = vColor[1];
    gl_Position.xyz = c;
    EmitVertex();

    EndPrimitive();

    gColor          = vColor[0];
    gl_Position.xyz = b;
    EmitVertex();

    gColor          = vColor[1];
    gl_Position.xyz = c;
    EmitVertex();

    gColor          = vColor[1];
    gl_Position.xyz = d;
    EmitVertex();

    EndPrimitive();
}
