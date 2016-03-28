out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D texColor;
uniform sampler2D texBloom;

void main()
{
    vec3 color = texture(texColor, varTexcoord).rgb + texture(texBloom, varTexcoord).rgb;
    fragColor = vec4(color, 1.0);
}

