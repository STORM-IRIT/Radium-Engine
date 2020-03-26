layout( location = 0 ) in vec3 in_position;
layout( location = 5 ) in vec4 in_color;

out vec4 vColor;
out vec3 vPosition;

struct Transform {
    mat4 model;
    mat4 view;
    mat4 proj;
};

uniform Transform transform;

void main() {
    mat4 mvp    = transform.proj * transform.view * transform.model;
    vec4 pos    = mvp * vec4( in_position.xyz, 1.0 );
    gl_Position = pos;
    vColor      = in_color;
    vPosition   = pos.xyz / pos.w;
}
