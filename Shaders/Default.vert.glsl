layout (location = 0) in vec3 pos;

struct Transform
{
    mat4 mvp;
};

uniform Transform transform;

void main()
{
    gl_Position = transform.mvp * vec4(pos, 1.0);
}
