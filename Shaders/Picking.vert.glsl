layout (location = 0) in vec3 inPosition;


struct Transform
{
    mat4 model;
    mat4 view;
    mat4 proj;
};


uniform int drawFixedSize;
uniform Transform transform;

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

    gl_Position = mvp * vec4(inPosition, 1.0);
}
