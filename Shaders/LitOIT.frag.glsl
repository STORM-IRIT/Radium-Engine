layout (location = 0) out vec4 f_Accumulation;
layout (location = 1) out vec4 f_Revealage;

#include "Structs.glsl"

uniform Material material;
uniform Light light;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_texcoord;
layout (location = 3) in vec3 in_eye;
layout (location = 4) in vec3 in_tangent;

#include "LightingFunctions.glsl"

void main()
{
    if (toDiscard() || material.alpha < 0.01)
    {
        discard;
    }
    
    float a = material.alpha;
    float z = -in_position.z;
    
    float va = (a + 0.01f);
    float va2 = va * va;
    float va4 = va2 * va2; // Pow4
    
    float vz = abs(z) / 200.0f;
    float vz2 = vz * vz;
    float vz4 = vz2 * vz2;

    float w = va4 + clamp(0.3f / (0.00001f + vz4), 0.01, 3000.0);

    f_Accumulation = vec4(computeLighting() * a, a) * w;
    f_Revealage    = vec4(a);
}


