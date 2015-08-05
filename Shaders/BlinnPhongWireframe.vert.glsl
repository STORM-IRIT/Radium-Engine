layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
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

out vec3 varPosition;
out vec3 varNormal;
out vec3 varEye;

void main()
{
    gl_Position = transform.mvp * vec4(inPosition, 1.0);

    vec4 pos = transform.model * vec4(inPosition, 1.0);
    pos /= pos.w;
    vec4 normal = transform.worldNormal * vec4(inNormal, 0.0);

    vec3 eye = -view[3].xyz * mat3(view);

    varPosition = vec3(pos);
    varNormal   = vec3(normal);
    varEye = vec3(eye);
}
