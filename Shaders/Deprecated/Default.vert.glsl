#include "Structs.glsl"

layout( location = 0 ) in vec3 in_position;

uniform Transform transform;

void main() {
    gl_Position = transform.proj * transform.view * transform.model * vec4( in_position, 1.0 );
}
