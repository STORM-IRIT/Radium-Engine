#include "Structs.glsl"


layout (location = 0) in vec3 pos;

uniform Transform transform;

void main()
{
    gl_Position = transform.proj * transform.view * transform.model * vec4(pos, 1.0);
}
