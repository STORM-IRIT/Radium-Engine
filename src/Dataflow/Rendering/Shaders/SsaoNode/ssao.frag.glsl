#include "TransformStructs.glsl"

layout (location = 0) out vec4 out_ssao;

uniform sampler2D normal_sampler;
uniform sampler2D position_sampler;
uniform sampler2DRect dir_sampler;

uniform Transform transform;
uniform float ssdoRadius;

// TODO compute a bias "a la" glPolygonOffset so that it adapts to z dynamic range.
const float bias = 0.001;

void main() {
    // Convert screen space position to world space position
    vec2 size = vec2(textureSize(position_sampler, 0));

    // The point to shade
    vec4 ptInWorld = texelFetch(position_sampler, ivec2(gl_FragCoord.xy), 0);
    float ssdo = 0;
    if (ptInWorld.w != 0) {
        int nbSamples = textureSize(dir_sampler).x;
        // The point normal (0 if the fragment must be discarded)
        vec3 nrmInWorld = texelFetch(normal_sampler, ivec2(gl_FragCoord.xy), 0).xyz;
        float fragDepth = (transform.view * vec4(ptInWorld.xyz, 1)).z;
        nrmInWorld = nrmInWorld * 2. -1.;

        for (int i=0;i<nbSamples; ++i) {
            // here the 0 in the texture coordinate may represent another sampling scheme
            vec4 offset = texelFetch(dir_sampler, ivec2(i, 0));
            // only keep samples in the rignt hemisphere (this introduce a random rotation in the sampling process)
            if (dot(offset.xyz, nrmInWorld) > 0) {
                offset *= offset.w * ssdoRadius;
                // theSample is the world position, displaced and reprojected.
                // It only serve to access the fragment for whihc the depth must be compared.
                vec4 theSample = vec4(ptInWorld.xyz + offset.xyz, 1);

                theSample = transform.mvp * theSample;
                theSample.xyz /= theSample.w;
                theSample.xyz = theSample.xyz * 0.5 + 0.5;

                vec4 sampledPoint = texture(position_sampler, theSample.st);
                if (sampledPoint.w != 0) {
                    float sampleDepth = (transform.view * sampledPoint).z;

                    float rangeCheck = smoothstep(0.0, 1.0, ssdoRadius / abs(fragDepth - sampleDepth));
                    ssdo       += (sampleDepth >= fragDepth + bias ? 1.0 : 0.0) * rangeCheck;
                }
            }
        }
        ssdo = 1 - ssdo/nbSamples;
        //ssdo *= ssdo;
        ssdo = smoothstep(0, 1, ssdo);
    }
    out_ssao = vec4(vec3(ssdo), 1);
}
