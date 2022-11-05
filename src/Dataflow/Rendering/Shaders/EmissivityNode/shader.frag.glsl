#include "VertexAttribInterface.frag.glsl"
//------------------- main ---------------------
uniform sampler2D amb_occ_sampler;

layout (location = 0) out vec4 out_color;
void main() {
    // discard non opaque fragment
    vec4 bc = getBaseColor(material, getPerVertexTexCoord());
    if (toDiscard(material, bc)) {
        discard;
    }

    vec2 size = textureSize(amb_occ_sampler, 0).xy;
    vec3 ao = texture(amb_occ_sampler, gl_FragCoord.xy/size).rgb;

    out_color  = vec4(bc.rgb * 0.01 * ao + getEmissiveColor(material, getPerVertexTexCoord()), 1.0);
}
