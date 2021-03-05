in vec4 gColor;
out vec4 fragColor;
in float pixelWidthDiv2;

const float PI = 3.1415926535897932384626433832795;

// AA is one pixel wide after pixelWidth of fully filled.

float aa( in float dist ) {
    float R = sqrt( 2. * .5 * .5 );
    float s = sign( dist );
    float d = max( min( dist - pixelWidthDiv2 + s * R, R ), -R );

    float theta = 2. * acos( max( min( d / R, 1. ), -1 ) );
    return clamp( R * R / 2. * ( theta - sin( theta ) ) / ( R * R * PI ), 0., 1. );
}
void main() {
    float a   = aa( gColor.a ) * aa( -gColor.a );
    fragColor = vec4( vec3( .8, .9, 1. ), a );
}
