#include "TransformStructs.glsl"

in gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
}
gl_in[];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

layout( location = 0 ) in vec3 in_position[];
layout( location = 1 ) in vec3 in_normal[];
layout( location = 2 ) in vec3 in_eye[];

uniform Transform transform;
// 0.01 : default value, but will be set when rendering point cloud for picking
uniform float pointCloudSplatRadius = 0.01;

layout( location = 0 ) out vec3 out_position;
layout( location = 1 ) out vec3 out_normal;
layout( location = 2 ) out vec3 out_eye;
layout( location = 3 ) flat out int out_eltID;
layout( location = 4 ) out vec3 out_eltCoords;

void main() {
    vec3 eye = -transform.view[3].xyz * mat3( transform.view );

    // if no normal is provided, splats are aligned with the screen plane
    vec3 normal = in_normal[0];
    if ( length( normal ) < 0.1 ) { normal = normalize( eye - in_position[0] ); }

    // orthonormal basis {u, v, normal}
    vec3 u = vec3( 1, 0, 0 );
    if ( abs( normal.x ) > 1e-3 ) { u = normalize( vec3( -normal.z / normal.x, 0, 1 ) ); }
    vec3 v = normalize( cross( normal, u ) );

    // (-1,+1)      (0,+1)     (+1,+1)
    //        +-------+-------+
    //        |1      |      3|
    //        |       |       |
    //        |       |       |
    // (-1,0) +-------+-------+ (+1,0)
    //        |       |       |
    //        |       |       |
    //        |0      |      2|
    //        +-------+-------+
    // (-1,-1)      (0,-1)     (+1,-1)

    // quad corners
    vec3 point[4];
    point[0] = in_position[0] - pointCloudSplatRadius * ( u + v );
    point[1] = point[0] + pointCloudSplatRadius * u * 2;
    point[2] = point[0] + pointCloudSplatRadius * v * 2;
    point[3] = point[0] + pointCloudSplatRadius * ( u + v ) * 2;

    // uv coordinates
    vec2 uv[4];
    uv[0] = vec2( -1, -1 );
    uv[1] = vec2( -1, +1 );
    uv[2] = vec2( +1, -1 );
    uv[3] = vec2( +1, +1 );

    for ( int idx = 0; idx < 4; ++idx )
    {
        gl_Position   = transform.proj * transform.view * vec4( point[idx], 1 );
        out_position  = point[idx];
        out_normal    = normal;
        out_eye       = in_eye[0];
        out_eltID     = gl_PrimitiveIDIn;
        out_eltCoords = vec3( 1, 0, 0 );
        EmitVertex();
    }
    EndPrimitive();
}
