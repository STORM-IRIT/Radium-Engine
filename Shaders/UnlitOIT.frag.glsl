layout (location = 0) out vec4 f_Accumulation;
layout (location = 1) out vec4 f_Revealage;

#include "Structs.glsl"

uniform Material material;
uniform Light light;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
    vec3 color;
    mat3 TBN;
} fs_in;

#include "Helpers.glsl"

void main()
{
    if (toDiscard() || material.alpha < 0.01)
    {
        discard;
    }
    
    float a = material.alpha;
    float z = -fs_in.position.z;
    
    float va = (a + 0.01f);
    float va2 = va * va;
    float va4 = va2 * va2; // Pow4
    
    float vz = abs(z) / 200.0f;
    float vz2 = vz * vz;
    float vz4 = vz2 * vz2;

    float w = va4 + clamp(0.3f / (0.00001f + vz4), 0.01, 3000.0);

    vec3 color;
    if (material.tex.hasKd == 1)
    {
        color = getKd();        
    }
    else
    {
        color = fs_in.color;
    }
    
    f_Accumulation = vec4(color * a, a) * w;
    f_Revealage    = vec4(a);
}
