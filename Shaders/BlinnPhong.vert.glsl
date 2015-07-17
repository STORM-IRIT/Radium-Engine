layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
// TODO(Charly): Add other inputs

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 varPosition;
out vec3 varNormal;

void main()
{
    gl_Position = proj * view * model * vec4(inPosition, 1.0);

    varPosition = inPosition;
    varNormal   = inNormal;
}
