layout (location = 0) in vec3 pos;
// TODO(Charly): Add other inputs

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 varPosition;

void main()
{
    gl_Position = proj * view * model * vec4(pos, 1.0);

    varPosition = vec4(pos, 1.0);
}
