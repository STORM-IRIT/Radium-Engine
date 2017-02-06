layout (location = 4) in vec3 in_texcoord;
in vec3 barycentric;

out vec4 out_color;

uniform Material material;

float edgeFactor()
{
	vec3 d = fwidth(barycentric);
	vec3 a3 = smoothstep(vec3(0.0), d * 1.5, barycentric);
	return min(min(a3.x, a3.y), a3.z);
}

void main(void) 
{
	// determine frag distance to closest edge
	float fEdgeIntensity = 1.0 - edgeFactor();

	// blend between edge color and face color
	vec4 vFaceColor = vec4(0.5, 0.5, 0.5, 1.0);
	vec4 vEdgeColor = vec4(texture(material.tex.kd, in_texcoord.st).rgb, 1);
	out_color = mix(vFaceColor, vEdgeColor, fEdgeIntensity);
}
