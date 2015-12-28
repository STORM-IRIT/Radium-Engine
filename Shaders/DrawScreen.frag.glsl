out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D screenTexture;

void main()
{
    fragColor = vec4( texture(screenTexture, varTexcoord).xyz, 1.0 );
}
