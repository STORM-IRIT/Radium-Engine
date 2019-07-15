layout (location = 0) in vec3 in_position;
layout (location = 5) in vec3 in_color;

uniform mat4 view;
uniform mat4 proj;

out vec3 vCol;

void main()
{
    gl_Position = proj * view * vec4(in_position, 1.0);
    vCol = in_color;
}
