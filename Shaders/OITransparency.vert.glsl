layout (location = 0) in vec3 pos;
layout (location = 4) in vec3 texcoord;

struct Transform
{
    mat4 mvp;
};

uniform Transform transform;

out vec3 vViewPosition;
out vec3 vTexcoord;

out float vDepth;

void main()
{
    gl_Position = transform.mvp * vec4(pos, 1.0);
    vec4 pos = transform.mvp * vec4(pos, 1.0);
    vViewPosition = pos.xyz;
    vTexcoord = texcoord;
    vDepth = -vViewPosition.z;
}
