out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;
uniform sampler2D ambient;

void main()
{
    vec4 colorValue = vec4(texture(color, varTexcoord));
    vec4 ambientValue = vec4(texture(ambient, varTexcoord));

    fragColor = colorValue + ambientValue;
}

