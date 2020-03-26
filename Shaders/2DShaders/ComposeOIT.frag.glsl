in vec2 varTexcoord;

out vec4 f_Color;

uniform sampler2D u_OITSumColor;
uniform sampler2D u_OITSumWeight;

void main() {
    float r = texture( u_OITSumWeight, varTexcoord ).r;

    if ( r >= 1.0 ) { discard; }

    vec4 accum = texture( u_OITSumColor, varTexcoord );

    vec3 avg_color = accum.rgb / max( accum.a, 0.00001 );

    f_Color = vec4( avg_color, r );
    // f_Color = vec4(accum.rgb, r);
    // f_Color = vec4(1, 1, 1, 1);
}
