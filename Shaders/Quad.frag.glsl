in vec2 varTexcoord;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_position;
layout (location = 3) out vec4 fragColor;

uniform bool useNormal;
uniform int neighSize;
uniform float depthThresh;
uniform vec2 WindowSize;
uniform sampler2D position;
uniform sampler2D normal;

layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_position;
float xStep, yStep;
float maxSqrDis;

float weight(float sqrDis)
{
    if (neighSize == 0) return 1;
    return maxSqrDis >= sqrDis ? pow((1-sqrDis/maxSqrDis),4) : 0;//(maxDis-squareDis)*(maxDis-squareDis)*(maxDis-squareDis)*(maxDis-squareDis) : 0;
}

void main()
{
    xStep = 1/WindowSize.x;
    yStep = 1/WindowSize.y;

    float sum =0, depth = 1000;
    vec3 c = vec3(0, 0, 0);
    float num =0;
    maxSqrDis = ((neighSize) * xStep * (neighSize) * xStep);// + (neighSize) * yStep * (neighSize) * yStep);

    float xStart = varTexcoord.x - neighSize * xStep, xEnd = varTexcoord.x + neighSize * xStep;
    float yStart = varTexcoord.y - neighSize * yStep, yEnd = varTexcoord.y + neighSize * yStep;
//finding minimum depth in the neighborhood
    for (float i=xStart; i <= xEnd; i+=xStep)
        for (float j=yStart; j <= yEnd; j+=yStep)
        {
            float sqrDis = distance(varTexcoord, vec2(i,j));
            sqrDis = sqrDis*sqrDis;
            if (sqrDis <= maxSqrDis)
            {
                vec3 tmpColor = texture(position, vec2(i,j)).xyz;
                if(length(tmpColor) > 0.01 && tmpColor.z < depth)
                {
                    vec3 tmpNormal = texture(normal, vec2(i,j)).xyz;
                    if(tmpNormal.z < 0) depth = tmpColor.z;
                }
            }
        }
    //depth/=num;
    for (float i=xStart; i <= xEnd; i+=xStep)
        for (float j=yStart; j <= yEnd; j+=yStep)
        {
            float sqrDis = distance(vec2(varTexcoord), vec2(i,j));
            sqrDis = sqrDis*sqrDis;
            if (sqrDis <= maxSqrDis)
            {
                vec3 tmpColor = texture(position, vec2(i,j)).xyz;
                if(length(tmpColor) > 0.01 && abs(tmpColor.z - depth) <= depthThresh )
                {
                    vec3 tmpNormal = texture(normal, vec2(i,j)).xyz;
                    float w = weight(sqrDis);
                    if(tmpNormal.z < 0)
                    {
                        c += w * (useNormal ? tmpNormal:tmpColor);
                        sum += w;
                    }
                }
            }
        }

    if(!useNormal) fragColor = vec4(c/sum, 1.0);
    else fragColor = vec4(normalize(vec3(c.xy, -c.z)), 1.0);
    out_normal = vec4(in_normal, 1.0);
    out_position = vec4(in_position, 1.0);
}
