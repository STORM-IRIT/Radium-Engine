layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec3 in_position[];
layout (location = 4) in vec3 in_texcoord[];

layout (location = 4) out vec3 out_texcoord;
out vec3 barycentric;

void main(void)
{
	barycentric = vec3(1, 0, 0);
	out_texcoord = in_texcoord[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	barycentric = vec3(0, 1, 0);
	out_texcoord = in_texcoord[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	barycentric = vec3(0, 0, 1);
	out_texcoord = in_texcoord[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}
