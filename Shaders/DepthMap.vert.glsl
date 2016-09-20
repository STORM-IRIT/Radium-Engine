#include "Structs.glsl"

layout (location = 0) in vec3 inPosition;
// TODO(Charly): Add other inputs

uniform mat4 lightMatrix;
uniform mat4 model;

void main()
{
    mat4 mvp = lightMatrix * model;
    gl_Position = mvp * vec4(inPosition, 1.0);
}
