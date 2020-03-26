#include "Structs.glsl"

out vec4 fragColor;

uniform Material material;

void main() {
    fragColor = vec4( material.kd.xyz, 1.0 );
}
