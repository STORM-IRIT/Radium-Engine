#include "TransformStructs.glsl"

//This is for a preview of the shader composition, but in time we must use more specific Light Shader
#include "DefaultLight.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_tangent;
layout (location = 3) in vec3 in_bitangent;
layout (location = 4) in vec3 in_texcoord;
layout (location = 5) in vec4 in_color;

uniform Transform transform;

uniform mat4 uLightSpace;

layout (location = 0) out vec3 out_position;
layout (location = 2) out vec3 out_texcoord;
layout (location = 5) out vec3 out_eyeInModelSpace;
layout (location = 6) out vec3 out_lightVector;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(in_position, 1.0);

    // eye in world space
    vec3 eye = -transform.view[3].xyz * mat3(transform.view);
    // eye in model space
    out_eyeInModelSpace = (inverse(transform.model) * vec4(eye, 1.)).xyz;

    // position in modelspace
    out_position    = in_position;
    out_texcoord    = in_texcoord;
}
