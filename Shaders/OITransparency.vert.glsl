layout (location = 0) in vec3 pos;
layout (location = 4) in vec3 texcoord;

uniform mat4 mvp = mat4(1.0);

out vec3 vViewPosition;
out vec3 vTexcoord;

out float vDepth;

void main()
{
    gl_Position = mvp * vec4(pos, 1.0);
    vec4 pos = mvp * vec4(pos, 1.0);
    vViewPosition = pos.xyz;
    vTexcoord = texcoord;
    vDepth = -vViewPosition.z;
}
