layout (location = 0) in vec3 position;

out vec2 varTexcoord;

void main()
{
    gl_Position = vec4(position, 1.0);
    varTexcoord = (position.xy + 1.0) / 2.0;
}
