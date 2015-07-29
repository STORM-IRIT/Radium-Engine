out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D renderpassColor;

void main()
{
    vec4 color = vec4(texture(renderpassColor, varTexcoord));

    fragColor = color;
}

