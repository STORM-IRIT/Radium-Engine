#include "TransformStructs.glsl"

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec3 in_tangent;
layout( location = 3 ) in vec3 in_bitangent;
layout( location = 4 ) in vec3 in_texcoord;
layout( location = 5 ) in vec4 in_color;

uniform Transform transform;

layout( location = 0 ) out vec3 out_position;
layout( location = 1 ) out vec3 out_eyeInModelSpace;

// The modeltoimage matrix
flat out mat4 invbiasmvp;
flat out mat4 world2model;

void main() {
    mat4 mvp    = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4( in_position, 1.0 );

    // Done in Fragment
    mat4 bias =
        mat4( 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0 );
    invbiasmvp  = inverse( bias * mvp );
    world2model = inverse( transform.model );

    // eye in world space
    vec3 eye = -transform.view[3].xyz * mat3( transform.view );
    // eye in model space
    out_eyeInModelSpace = ( world2model * vec4( eye, 1. ) ).xyz;

    // position in modelspace
    out_position = in_position;
}
