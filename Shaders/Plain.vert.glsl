layout (location = 0) in vec3 inPos;
layout (location = 4) in vec3 inTexcoord;
layout (location = 5) in vec4 inColor;

struct Transform
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

uniform Transform transform;
uniform int drawFixedSize;

out vec3 vTexcoord;
out vec4 vColor;

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

    gl_Position = mvp * vec4(inPos.xyz, 1.0);
    vColor = inColor;
	vTexcoord = inTexcoord;
}
