#include "Structs.glsl"

layout (location = 0)      in vec3 in_position;
layout (location = 1)      in vec3 in_normal;
layout (location = 2)      in vec3 in_eye;
layout (location = 3) flat in int  in_eltID;
layout (location = 4)      in vec3 in_eltCoords;

out ivec4 fragId;

uniform int objectId;

void main()
{
    // dot product between view and normal
    float angle = dot( in_eye - in_position, in_normal );

    // discard if culled
    if( angle <= 0 )
    {
        discard;
    }

    // set Object ID
    fragId.r = objectId;
    // set Vertex ID in element
    if( in_eltCoords.x > in_eltCoords.y && in_eltCoords.x > in_eltCoords.z)
        fragId.g = 0;
    if( in_eltCoords.y > in_eltCoords.x && in_eltCoords.y > in_eltCoords.z)
        fragId.g = 1;
    if( in_eltCoords.z > in_eltCoords.x && in_eltCoords.z > in_eltCoords.y)
        fragId.g = 2;
    // set Element ID
    fragId.b = in_eltID;
    // set Edge opposite vertex, valid only if triangle element
    if( in_eltCoords.x < in_eltCoords.y && in_eltCoords.x < in_eltCoords.z)
        fragId.a = 0;
    if( in_eltCoords.y < in_eltCoords.x && in_eltCoords.y < in_eltCoords.z)
        fragId.a = 1;
    if( in_eltCoords.z < in_eltCoords.x && in_eltCoords.z < in_eltCoords.y)
        fragId.a = 2;
}
