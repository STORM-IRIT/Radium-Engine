out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D volumeImage;

void main() {
    vec2 size     = vec2( textureSize( volumeImage, 0 ) );
    vec4 volColor = texelFetch( volumeImage, ivec2( varTexcoord.xy * size ), 0 );
    if ( volColor.a < 1 ) discard;
    fragColor = vec4( volColor.rgb, 0 );
}
