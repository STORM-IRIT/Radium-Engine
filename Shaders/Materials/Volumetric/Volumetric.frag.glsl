// Fragment position in model space
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec3 in_texcoord;
// view position in model space
layout (location = 5) in vec3 in_eyeInModelSpace;
layout (location = 6) in vec3 in_lightVector;

#include "Volumetric.glsl"

out vec4 fragColor;

void main() {

    // Transform all informations in the canonical grid space
    vec4 pos = material.modelToDensity * vec4(in_position, 1);
    vec4 eye = material.modelToDensity * vec4(in_eyeInModelSpace, 1);

    // compute the ray dir in model space
    vec3 rayDir = normalize(pos.xyz - eye.xyz);

    vec3 volTransmitance;
    vec3 volColor = raymarch(material, in_position, rayDir, in_lightVector, volTransmitance);
    // Take into account the existing color modulated by the trasmittance
    // vec3 backgroungColor = volTransmitance * existingfragmentcolor;
    fragColor = vec4(volColor, 1);
}

