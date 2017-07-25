layout (location = 0) out vec4 out_position;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 secondary_position;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
in vec3 transformed_position;
in vec3 transformed_normal;

void main()
{
    out_position = vec4(transformed_position, 1.0);
    secondary_position = out_position;
    out_normal = vec4(transformed_normal, 1.0);
}
