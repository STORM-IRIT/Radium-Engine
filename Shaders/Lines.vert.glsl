/* RADIUM_SHADER_GLOBAL_REPLACE_VERTEX */

layout (location = 0) in vec3 inPos;
layout (location = 5) in vec4 inColor;

out vec4 vColor;
out vec3 vPosition;

struct Transform
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

uniform Transform transform;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(inPos.xyz, 1.0);
    vColor = inColor;

    vec4 pos = mvp * vec4(inPos.xyz, 1.0);
    vPosition = pos.xyz / pos.w;
}
