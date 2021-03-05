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

in vec4 vPosition[2];
// in vec4 vColor[2];

out vec4 gColor;
out float pixelWidthDiv2;
out float slopeY2;
uniform vec2 viewport;

void main() {
    vec3 vp = vec3( viewport, 1. );
    vec3 p0 = vPosition[0].xyz / vPosition[0].w * vp;
    vec3 p1 = vPosition[1].xyz / vPosition[1].w * vp;

    float pixelWidth = 3.;
    pixelWidthDiv2   = pixelWidth / 2.;
    vec2 slope       = normalize( vec2( p1.xy - p0.xy ) );
    slopeY2          = slope.y * slope.y;
    vec3 n           = ( pixelWidth + 2 ) * normalize( vec3( -slope.y, slope.x, 0 ) );

    vec4 a = vec4( ( p0 + n ) / vp, 1. );
    vec4 b = vec4( ( p0 - n ) / vp, 1. );
    vec4 c = vec4( ( p1 + n ) / vp, 1. );
    vec4 d = vec4( ( p1 - n ) / vp, 1. );

    vec4 vColor[2];
    vColor[0] = vec4( vec3( .7 ), -pixelWidthDiv2 - 1. );
    vColor[1] = vec4( vec3( .7 ), +pixelWidthDiv2 + 1. );

    gColor      = vColor[0];
    gl_Position = a;
    EmitVertex();

    gColor      = vColor[1];
    gl_Position = b;
    EmitVertex();

    gColor      = vColor[0];
    gl_Position = c;
    EmitVertex();

    gColor      = vColor[1];
    gl_Position = d;
    EmitVertex();

    EndPrimitive();
}
