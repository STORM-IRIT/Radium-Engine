layout (location = 0) in vec3 pos;
layout (location = 4) in vec3 texcoord;

struct Transform
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

uniform Transform transform;

out vec3 vViewPosition;
out vec3 vTexcoord;

out float vDepth;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(pos, 1.0);
    vec4 pos = mvp * vec4(pos, 1.0);
    vViewPosition = pos.xyz;
    vTexcoord = texcoord;
    vDepth = -vViewPosition.z;
}
