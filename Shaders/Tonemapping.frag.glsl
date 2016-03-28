in vec2 varTexcoord;

out vec4 fragColor;

uniform sampler2D hdr;

uniform float lumMin;
uniform float lumMax;
uniform float lumMean;

#include "Tonemap.glsl"

void main()
{
    vec3 color = texture(hdr, varTexcoord).rgb;

    vec3 Yxy = rgb2Yxy(color);

    // FIXME(charly): 2.0 might be too high with many lights (or very shiny ones). Reinhard's suggestion is 0.18. VortexEngine uses 1.03
    float middleGrey = getMiddleGrey(lumMean);
    float lumScaled = getLumScaled(Yxy.r, middleGrey, lumMean);
    float white = max(2 * lumMean, lumMax);
    float lumCompressed = (lumScaled * (1.0 + lumScaled / (white * white))) / (1.0 + lumScaled);
    Yxy.r = lumCompressed;

    color = Yxy2rgb(Yxy);

    fragColor = vec4(color, 1.0);
}
