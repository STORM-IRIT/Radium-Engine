in vec2 varTexcoord;
out vec4 fragColor;

uniform sampler2D hdr;

uniform float lumMin;
uniform float lumMax;
uniform float lumMean;

#include "Tonemap.glsl"

void main()
{
    vec2 size = vec2(textureSize(hdr, 0));
    vec3 color = texelFetch(hdr, ivec2(varTexcoord * size), 0).rgb;

    vec3 Yxy = rgb2Yxy(color);

    float middleGrey = getMiddleGrey(lumMean);
    float lumScaled = getLumScaled(Yxy.r, middleGrey, lumMean);

    float white = 1.0;

    // TODO(charly): Uniforms ?
    float T = 0.5;
    float O = 2.0;

    float lumThreshold = max(lumScaled * (1 + lumScaled) - T, 0.0);
    float lumBright = lumThreshold / (O + lumThreshold);

    Yxy.r = lumBright;
    color = Yxy2rgb(Yxy);

    if (isnan(color.r) || isnan(color.g) || isnan(color.b))
        color = vec3(0.0) ;

    fragColor = vec4(color, 1.0);
}
