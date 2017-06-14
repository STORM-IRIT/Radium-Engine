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

    float grey = getMiddleGrey(lumMean);
    float white = getWhite(lumMean, lumMax);
    float scaled = getLumScaled(Yxy.r, grey, lumMean);
    float compressed = getLumCompressed(scaled, white);
    Yxy.r = compressed;

    color = Yxy2rgb(Yxy);

    fragColor = vec4(color, 1.0);
}
