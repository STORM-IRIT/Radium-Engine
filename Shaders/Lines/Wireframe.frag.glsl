in vec4 gColor;
out vec4 fragColor;
in float pixelWidthDiv2;
in float slopeY2;

// some sort of https://www.geeksforgeeks.org/anti-aliased-line-xiaolin-wus-algorithm/
// by reconstructing a rectangle triangle
// AA is one pixel wide after pixelWidth of fully filled.
void main() {
    float d = max( abs( gColor.a ) - pixelWidthDiv2 + 0.5, 0. );
    float a = clamp( 1 - d * sqrt( 1. + slopeY2 ), 0., 1. );

    fragColor = vec4( vec3( 1., 0., 0. ), a );
}
