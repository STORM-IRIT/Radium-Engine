layout (location = 0) in vec3 pos;

uniform mat4 mvp;

void main()
{
    mat4 theRealMvp = mvp;
    if (mvp == mat4(0))
    {
        theRealMvp = mat4(1.0);
    }

    gl_Position = mvp * vec4(pos, 1.0);
}
