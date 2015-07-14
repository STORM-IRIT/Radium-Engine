out vec4 fragColor;

in vec4 varPosition;

void main()
{
    vec3 color = varPosition.xyz;
    color = color * 0.67;
    color = color + vec3(0.5, 0.5, 0.0);
    fragColor = vec4(color, 1.0);
}
