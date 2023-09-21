#ifndef LINEARBLENDSKINNING_GLSL
#define LINEARBLENDSKINNING_GLSL

#ifdef LINEAR_BLEND_SKINNING

layout( location = 6 ) in vec4 in_weight;
layout( location = 7 ) in uvec4 in_weight_idx;

uniform mat4 joints[];

mat4 skinMatrix() {
	return in_weight.x * joints[in_weight_idx.x] +
		in_weight.y * joints[in_weight_idx.y] +
		in_weight.z * joints[in_weight_idx.z] +
		in_weight.w * joints[in_weight_idx.w];
}

#else

mat4 skinMatrix() {
	return mat4(1.0);
}

#endif // LINEAR_BLEND_SKINNING

#endif // DEFAULTLIGHTSHADER_GLSL
