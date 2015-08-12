layout (location = 0) in vec3 inPosition;

struct Transform
{
    mat4 mvp;
    mat4 model;
    mat4 worldNormal;
};

uniform Transform transform;

void main()
{
    gl_Position = transform.mvp * vec4(inPosition, 1.0);
}
