layout (location = 0) in vec3 inPosition;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(inPosition, 1.0);
}
