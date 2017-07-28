//#include "Structs.glsl"

layout (location = 0) out vec4 out_position;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 secondary_position;

//uniform Transform transform;

in vec3 transformed_position;
in vec3 transformed_normal;

void main()
{
    out_normal = vec4(transformed_normal, 1.0);
    out_position = vec4(transformed_position, 1.0);
    secondary_position = out_position;
}
