
#include "TransformStructs.glsl"

//This is for a preview of the shader composition, but in time we must use more specific Light Shader
#include "DefaultLight.glsl"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 4) in vec3 in_texcoord;
layout (location = 5) in vec4 in_color;

uniform Transform transform;
uniform int drawFixedSize = 0;


layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_texcoord;
layout (location = 2) out vec3 out_normal;
layout (location = 5) out vec3 out_lightVector;
layout (location = 6) out vec3 out_vertexColor;

void main()
{
    mat4 mvp;
    if ( drawFixedSize > 0 )
    {
        // distance to camera
        mat4 modelView = transform.view * transform.model;
        float d = length( modelView[3].xyz );
        mat3 scale3 = mat3(d);
        mat4 scale = mat4(scale3);
        mat4 model = transform.model * scale;
        mvp = transform.proj * transform.view * model;
    }
    else
    {
        mvp = transform.proj * transform.view * transform.model;
    }

    gl_Position = mvp * vec4(in_position, 1.0);
    out_vertexColor = in_color.rgb;
    out_texcoord = in_texcoord;

    vec4 pos = transform.model * vec4(in_position, 1.0);
    pos /= pos.w;
    out_position = vec3(pos);

    vec3 normal = mat3(transform.worldNormal) * in_normal;
    out_normal      = normal;

    out_lightVector = getLightDirection(light, out_position);
}
