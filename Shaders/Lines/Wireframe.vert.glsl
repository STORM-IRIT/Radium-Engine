#include "TransformStructs.glsl"

layout( location = 0 ) in vec3 in_position;
out vec4 vPosition;

uniform Transform transform;

void main() {
    mat4 mvp    = transform.proj * transform.view * transform.model;
    vec4 pos    = mvp * vec4( in_position.xyz, 1.0 );
    gl_Position = pos;
    vPosition   = pos;
}
