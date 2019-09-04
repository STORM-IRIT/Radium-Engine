layout (location = 0) out vec4 out_ambient;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_diffuse;
layout (location = 3) out vec4 out_specular;

#include "TransformStructs.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_viewVector;
layout (location = 5) in vec3 in_lightVector;
layout (location = 6) in vec3 in_color;

#include "BlinnPhongMaterial.glsl"

//------------------- main ---------------------
void main() {
    // TODO use diffuse texture alpha channel combined with material.alpha ?
    // Discard non fully opaque fragments
    if (toDiscard(material, in_texcoord.xy ) || (material.alpha < 1) )
        discard;

	vec3 binormal 	 = normalize(cross(in_normal, in_tangent));
	vec3 localNormal = getNormal(material, in_texcoord.xy, in_normal, in_tangent, binormal);


    out_ambient 	= vec4(getKd(material, in_texcoord.xy) * 0.1, 1.0);
    out_normal 		= vec4(localNormal * 0.5 + 0.5, 1.0 );
    out_diffuse 	= vec4(getKd(material, in_texcoord.xy), 1.0);
    out_specular 	= vec4(getKs(material, in_texcoord.xy), 1.0);
}
