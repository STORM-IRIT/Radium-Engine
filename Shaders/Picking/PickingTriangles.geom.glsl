layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

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

layout( location = 0 ) in vec3 in_position[3];
layout( location = 1 ) in vec3 in_normal[3];
layout( location = 2 ) in vec3 in_eye[3];

layout( location = 0 ) out vec3 out_position;
layout( location = 1 ) out vec3 out_normal;
layout( location = 2 ) out vec3 out_eye;
layout( location = 3 ) flat out int out_eltID;
layout( location = 4 ) out vec3 out_eltCoords;

void main() {
    // a
    gl_Position   = gl_in[0].gl_Position;
    out_position  = in_position[0];
    out_normal    = in_normal[0];
    out_eye       = in_eye[0];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3( 1, 0, 0 );
    EmitVertex();

    // b
    gl_Position   = gl_in[1].gl_Position;
    out_position  = in_position[1];
    out_normal    = in_normal[1];
    out_eye       = in_eye[1];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3( 0, 1, 0 );
    EmitVertex();

    // c
    gl_Position   = gl_in[2].gl_Position;
    out_position  = in_position[2];
    out_normal    = in_normal[2];
    out_eye       = in_eye[2];
    out_eltID     = gl_PrimitiveIDIn;
    out_eltCoords = vec3( 0, 0, 1 );
    EmitVertex();
}
