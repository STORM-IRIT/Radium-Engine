#include "Structs.glsl"

uniform Material material;
uniform Light light;

in VS_OUT
{
    vec3 normal;
    vec3 texcoord;
    vec3 color;
} fs_in;

out vec4 fragColor;

#include "Helpers.glsl"


void main()
{
    if ( material.tex.hasKd == 1 )
    {
        vec3 color = getKd();
        if (toDiscard()) discard;
        fragColor = vec4(color,1);
    }
    else
    {
        fragColor = vec4( fs_in.color.rgb, 1.0);
    }
}

