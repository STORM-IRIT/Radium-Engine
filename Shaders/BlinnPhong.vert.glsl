layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec3 inTexcoord;
// TODO(Charly): Add other inputs

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

out vec3 vPosition;
out vec3 vNormal;
out vec3 vTexcoord;
out vec3 vEye;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(inPosition, 1.0);

    vec4 pos = transform.model * vec4(inPosition, 1.0);
    pos /= pos.w;
    vec4 normal = transform.worldNormal * vec4(inNormal, 0.0);

    vec3 eye = -transform.view[3].xyz * mat3(transform.view);

    vPosition = vec3(pos);
    vNormal   = vec3(normal);
    vEye      = vec3(eye);

    vTexcoord = inTexcoord;
}
