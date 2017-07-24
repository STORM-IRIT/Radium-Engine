layout (location = 0) out vec4 out_position;
layout (location = 1) out vec4 out_normal;

//#include "Structs.glsl"

//uniform Material material;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
in vec3 transformed_position;
in vec3 transformed_normal;
//layout (location = 2) in vec3 in_texcoord;
//layout (location = 3) in vec3 in_eye;
//layout (location = 4) in vec3 in_tangent;

//#include "Helpers.glsl"

void main()
{
    out_position = vec4(transformed_position, 1.0);//getKd() * 0.1, 1.0);
    out_normal = vec4(transformed_normal, 1.0);
}
