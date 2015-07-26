layout (location = 0) in vec3 pos;

uniform mat4 mvp = mat4(1.0);

void main()
{
    gl_Position = mvp * vec4(pos, 1.0);
}
