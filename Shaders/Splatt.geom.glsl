#include "Structs.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout (location = 0) in vec3 in_position[];
layout (location = 1) in vec3 in_normal[];


uniform Transform transform;
uniform vec2 WindowSize;
uniform float radius;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 4) out vec2 out_uv;

out gl_PerVertex
{
vec4 gl_Position;
};


void main()
{

    // orthonormal basis {in_normal, u, v}
    vec3 tmpNormal = normalize(in_normal[0]);
    vec3 u = mix(vec3(1,0,0), normalize(vec3(-tmpNormal.z/tmpNormal.x, 0, 1)), abs(tmpNormal.x)>0.0001);
    vec3 v = normalize(cross(tmpNormal, u));

    // quad corners and uv coordinates
    vec3 point[4];
    vec2 uv[4];
    float l_radius = radius / WindowSize.x;

    point[0] = in_position[0] - l_radius*(u+v);
    uv[0] = vec2(-1,-1);

    point[1] = point[0] + l_radius*u*2;
    uv[1] = vec2(-1,+1);

    point[2] = point[0] + l_radius*v*2;
    uv[2] = vec2(+1,-1);

    point[3] = point[0] + l_radius*(u+v)*2;
    uv[3] = vec2(+1,+1);

    for(int idx = 0; idx<4; ++idx)
    {
        gl_Position  = transform.proj * vec4(point[idx],1);
        out_position = point[idx];
        out_normal   = tmpNormal;
        out_uv       = uv[idx];
        EmitVertex();
    }

    EndPrimitive();
}