in vec2 varTexcoord;

uniform int neighSize;
uniform sampler2D position;

layout (location = 0) out vec4 fragcolor;

float mask;

void main()
{
    mask = 0;
    int sz = 2;
    for (int i=-sz; i<=sz && mask == 0; i++)
        for (int j=-sz; j<=sz && mask == 0; j++)
            if (length(vec2(i,j)) <= sz && length(texelFetch(position, ivec2(gl_FragCoord) + ivec2(i, j), 0)) > 0.5) mask = 1;

    fragcolor = vec4(mask,0,0 , 1);
}
