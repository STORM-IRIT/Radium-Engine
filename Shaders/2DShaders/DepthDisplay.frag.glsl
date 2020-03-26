out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D screenTexture;

float LinearizeDepth( in vec2 uv ) {
    float zNear = 0.1;   // TODO: Replace by the zNear of the perspective projection
    float zFar  = 100.0; // TODO: Replace by the zFar  of the perspective projection
    float depth = texture( screenTexture, uv ).x;
    return ( 2.0 * zNear ) / ( zFar + zNear - depth * ( zFar - zNear ) );
}

void main() {
    float c   = LinearizeDepth( varTexcoord.xy );
    fragColor = vec4( c, c, c, 1.0 );
}
