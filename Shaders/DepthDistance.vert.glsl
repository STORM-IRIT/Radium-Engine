layout (location = 0) in vec3 in_position;

#include "Structs.glsl"
uniform Transform transform;

out float far;
out float near;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;

    // compute near and far values
    float m22 = -transform.proj[2][2];
    float m23 = -transform.proj[2][3];    
    far = m23/(m22+1);
    near = m23/(m22-1);

    gl_Position = mvp * vec4(in_position.xyz, 1.0);
}
