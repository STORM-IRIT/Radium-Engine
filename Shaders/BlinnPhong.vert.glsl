layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
// TODO(Charly): Add other inputs

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 mvp;

out vec3 varPosition;
out vec3 varNormal;
out vec3 varEye;

void main()
{
    gl_Position = mvp * vec4(inPosition, 1.0);

    mat4 modelView = view * model;
    mat4 normalMat = transpose(inverse(modelView));

    vec4 pos = model * vec4(inPosition, 1.0);
    pos /= pos.w;
    vec4 normal = model * vec4(inNormal, 0.0);

    vec3 eye = -view[3].xyz * mat3(view);

    varPosition = vec3(pos);
    varNormal   = vec3(normal);
    varEye = vec3(eye);
}
