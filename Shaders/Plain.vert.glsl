layout (location = 0) in vec3 inPos;
layout (location = 5) in vec4 inColor;

out vec4 vColor;

struct Transform
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 mvp;
    mat4 modelView;
    mat4 worldNormal;
    mat4 viewNormal;
};

uniform Transform transform;
uniform int drawFixedSize;

void main()
{
    mat4 mvp = transform.mvp;
    if ( drawFixedSize > 0 )
    {
        // distance to camera
        float d = length(transform.modelView[3].xyz);
        mat3 scale3 = mat3(d);
        mat4 scale = mat4(scale3);
        mat4 model = transform.model * scale;
        mvp = transform.proj * transform.view* model;
    }
    gl_Position = mvp * vec4(inPos.xyz, 1.0);
    vColor = inColor;
}
