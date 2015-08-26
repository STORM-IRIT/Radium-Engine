layout (location = 0) in vec3 inPos;
layout (location = 5) in vec4 inColor;

out vec4 vColor;
out vec3 vPosition;

struct Transform
{
    mat4 mvp;
};

uniform Transform transform;

void main()
{
    gl_Position = transform.mvp * vec4(inPos.xyz, 1.0);
    vColor = inColor;

    vec4 pos = transform.mvp * vec4(inPos.xyz, 1.0);
    vPosition = pos.xyz / pos.w;
}
