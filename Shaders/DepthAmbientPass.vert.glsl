layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec3 inTexcoord;

out vec3 vTexcoord;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(inPosition, 1.0);

    vTexcoord = inTexcoord;
}
