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
vec4 vColor[2];

float clipf( in float x0, in float x1, in float dx, in float w, in out int clipped ) {

    if ( w < x1 )
    {
        clipped = 1;
        return ( w - x0 ) / dx;
    }

    if ( x1 < -w )
    {
        clipped = -1;
        return ( -w - x0 ) / dx;
    }
    return 1.;
}

vec4 clipp( in vec3 dir, in vec4 p0, in vec4 p1, in out vec3 clipped ) {
    vec4 r = p1;
    r.xyz  = p0.xyz + clipf( p0.x, r.x, dir.x, r.w, clipped.x ) * dir;
    r.xyz  = p0.xyz + clipf( p0.y, r.y, dir.y, r.w, clipped.y ) * dir;
    r.xyz  = p0.xyz + clipf( p0.z, r.z, dir.z, r.w, clipped.z ) * dir;
    return r;
}

void main() {
    vec3 vp = vec3( viewport, 1. );
    vec3 c0 = vec3( 0 );
    vec3 c1 = vec3( 0 );

    vec4 css0 = gl_in[0].gl_Position;
    vec4 css1 = gl_in[1].gl_Position;

    vec4 scss0 = css0 * css1.w;
    vec4 scss1 = css1 * css0.w;

    vec3 dir = ( scss1 - scss0 ).xyz;
    vec4 ip0 = clipp( -dir, scss1, scss0, c0 );
    vec4 ip1 = clipp( dir, scss0, scss1, c1 );
    //  ip0 = scss0;
    // ip1 = scss1;
    float epsilon = 0.001;
    if ( ( c0.x != 0 && c0.x == c1.x ) || ( c0.y != 0 && c0.y == c1.y ) ||
         ( c0.z != 0 && c0.z == c1.z ) )
        return;
    vec3 p0 = ip0.xyz * vp;
    if ( abs( ip0.w ) > epsilon ) p0 /= ip0.w;
    vec3 p1 = ip1.xyz * vp;
    if ( abs( ip1.w ) > epsilon ) p1 /= ip1.w;

    float pixelWidth = 1.;
    pixelWidthDiv2   = pixelWidth / 2.;
    vec2 slope       = normalize( vec2( p1.xy - p0.xy ) );
    slopeY2          = slope.y * slope.y;
    vec3 n           = ( pixelWidth + 2 ) * normalize( vec3( -slope.y, slope.x, 0 ) );

    vec4 a = vec4( ( p0 + n ) / vp, 1. );
    vec4 b = vec4( ( p0 - n ) / vp, 1. );
    vec4 c = vec4( ( p1 + n ) / vp, 1. );
    vec4 d = vec4( ( p1 - n ) / vp, 1. );

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
