in vec2 varTexcoord;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_position;
layout (location = 3) out vec4 fragColor;

uniform vec2 WindowSize;
uniform sampler2D color;
uniform sampler2D normal;

layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_position;
float xStep, yStep;

float weight(vec2 src, vec2 neigh)
{
    //return 1;
    float squareDis = distance(src, neigh)*distance(src, neigh);
    return (squareDis-1)*(squareDis-1);
}

void main()
{
    xStep = 1/WindowSize.x;
    yStep = 1/WindowSize.y;

    int neighSize = 5;
    float sum =0;
    bool useNormal = true;
    vec3 c = vec3(0,0,0);

    float xStart = varTexcoord.x - neighSize * xStep, xEnd = varTexcoord.x + neighSize * xStep;
    float yStart = varTexcoord.y - neighSize * yStep, yEnd = varTexcoord.y + neighSize * yStep;
    for (float i=xStart; i <= xEnd; i+=xStep)
        for (float j=yStart; j <= yEnd; j+=yStep)
        {
            vec3 tmpColor;
            if (useNormal) tmpColor = texture(normal, vec2(i,j)).rgb;
            else tmpColor = texture(color, vec2(i,j)).rgb;

            if (length(tmpColor) > 0.1)
            {
                float w = weight(varTexcoord, vec2(i,j));
                c += w * tmpColor;
                sum += w;
            }

        }
    fragColor = vec4(c/sum, 1.0);
    out_normal = vec4(in_normal,0.1);
    out_position = vec4(in_position, 0.1);
}
