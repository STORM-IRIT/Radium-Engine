// Mostly copied from vortex engine's luminance shader

out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D hdr;

#include "Tonemap.glsl"

void main()
{
    vec3 color = texture(hdr, varTexcoord).rgb;

    float Y = luminance(color);
    float logY = log(Y + 0.0001);

    if (isnan(logY))
    {
        logY = 0.0;
    }

    fragColor = vec4(Y, Y, logY, 1.0);
}

