layout (location = 0) in vec3 inPos;
layout (location = 5) in vec4 inColor;

out vec4 vColor;

struct Transform
{
    mat4 mvp;
};

uniform Transform transform;

void main()
{
    gl_Position = transform.mvp * vec4(inPos.xyz, 1.0);
    vColor = inColor;
}
