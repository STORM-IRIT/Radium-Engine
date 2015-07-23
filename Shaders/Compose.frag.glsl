out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;

void main()
{
    vec4 value = vec4(texture(color, varTexcoord));
    fragColor = value;
}

