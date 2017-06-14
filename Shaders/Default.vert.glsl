layout (location = 0) in vec3 pos;

struct Transform
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

uniform Transform transform;

void main()
{
    gl_Position = transform.proj * transform.view * transform.model * vec4(pos, 1.0);
}
