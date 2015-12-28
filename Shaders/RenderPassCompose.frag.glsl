out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;

void main()
{
    fragColor = vec4( texture(color, varTexcoord).xyz, 1.0 );
}

