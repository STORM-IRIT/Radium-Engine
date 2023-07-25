layout(location = 0) out vec4 out_ambient;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_diffuse;
layout(location = 3) out vec4 out_specular;

#include "VertexAttribInterface.frag.glsl"
layout(location = 5) in vec3 in_viewVector;
layout(location = 6) in vec3 in_lightVector;

//------------------- main ---------------------
void main() {
    // discard non opaque fragment
    vec4 bc = getBaseColor(material, in_texcoord);

    if (toDiscard(material, bc)) discard;

    NormalInfo nrm_info = getNormalInfo(material.baseMaterial, in_texcoord);
    out_normal   = vec4(nrm_info.n * 0.5 + 0.5, 1.0);

// Unefficient but simple workaround to support unlit GLTF material
// This restrict unlit to opaque materials even if the spec allows unlit transparent
// TODO, modify radium renderers to support unlit opaque and transparent objects
#ifdef MATERIAL_UNLIT
    out_ambient = bc;
    out_diffuse  = vec4(bc.rgb, 1.0);
    out_specular = vec4(vec3(0.0), 1.0);
#else
    MaterialInfo bsdf_params;
    BsdfInfo layers;

    getSeparateBSDFComponent( material,
                              in_texcoord,
                              normalize(in_viewVector),
                              nrm_info,
                              bsdf_params,
                              layers );
    out_ambient  = vec4(layers.f_diffuse * 0.01 + getEmissiveColor(material, in_texcoord), 1.0);
    out_diffuse  = vec4(layers.f_diffuse, 1.0);
    out_specular = vec4(layers.f_specular, 1.0);
#endif
}
