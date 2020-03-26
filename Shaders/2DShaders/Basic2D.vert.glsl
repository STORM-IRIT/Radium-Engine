layout( location = 0 ) in vec3 in_position;

out vec2 varTexcoord;

void main() {
    gl_Position = vec4( in_position, 1.0 );
    varTexcoord = ( in_position.xy + 1.0 ) / 2.0;
}
