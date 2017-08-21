in vec2 varTexcoord;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_position;
layout (location = 3) out vec4 fragColor;

uniform bool showPos;
uniform bool planeFit;
uniform int neighSize;
uniform float depthThresh;
uniform int depthCalc;
uniform vec2 WindowSize;
uniform sampler2D position;
uniform sampler2D normal;

float INF = 30000, EPS = 0.00001;
float maxDis, depth;
float xStep, yStep;
float xStart, xEnd, yStart, yEnd;
vec3 posTex, normalTex, posOut, plane, WP, WN, u1to3, fit, tmpOut;
float sum, WPN, WPP, u4, u0;
int num;

float weight(float dis)
{
    //return 1;
    if (dis >= maxDis) return 0;
    float sqrX = dis/maxDis;
    sqrX = sqrX*sqrX;
    return pow((1-sqrX),4);
}

float mult(vec3 first, vec3 second)
{
    return first.x*second.x + first.y*second.y +first.z*second.z;
}

void init()
{
    normalTex = texture(normal, varTexcoord).xyz;
    posTex = texture(position, varTexcoord).xyz;

    xStep = 1/WindowSize.x;
    xStart = max(xStep, varTexcoord.x - neighSize * xStep);
    xEnd = min(1.0, varTexcoord.x + neighSize * xStep);

    yStep = 1/WindowSize.y;
    yStart = max(yStep, varTexcoord.y - neighSize * yStep);
    yEnd = min(1.0, varTexcoord.y + neighSize * yStep);

    maxDis = ((neighSize+1) * (xStep+EPS));
    depth = -INF;

    fit = tmpOut = posOut = plane = WP = WN = u1to3 = vec3(0);
    sum = WPN = WPP = u4 = u0 = 0;
    num = 0;
}

void findDepth()
{
    float self = 0, dis = 0, avDep = 0, sum = 0;
    vec3 tmpPos = texture(position, varTexcoord).xyz;
    if (length(tmpPos) > 0) self = tmpPos.z;
    for (float i=xStart; i<=xEnd; i+=xStep)
        for (float j=yStart; j<=yEnd; j+=yStep)
        {
            tmpPos = texture(position, vec2(i,j)).xyz;
            dis = distance(varTexcoord, vec2(i,j));
            if (dis < maxDis && length(tmpPos) > 0)
            {
                float w = weight(dis);
                if (tmpPos.z > depth) depth = tmpPos.z;
                avDep += tmpPos.z * w*w;
                sum += w*w;
            }
        }
   //if (self !=0 && abs(depth - self) < depthThresh) depth = self;
   switch(depthCalc)
   {
        case 0:
            break;
        case 1:
            depth = avDep/sum;
            break;
        default:
            break;
   }
}

void addNeighbors()
{
    for (float i=xStart; i<=xEnd; i+=xStep)
        for (float j=yStart; j<=yEnd; j+=yStep)
        {
            float dis = distance(vec2(varTexcoord), vec2(i, j));
            float w = weight(dis);
            vec3 neighPosTex = texture(position, vec2(i,j)).xyz;
            vec3 neighNormalTex = texture(normal, vec2(i,j)).xyz;
            vec3 relPos = vec3(varTexcoord, depth) - vec3(i, j, neighPosTex.z);
            if (dis < maxDis &&                 // point is in range
                length(neighPosTex) > 0 &&           // there actually is a point!
                relPos.z <= depthThresh/100 &&       // depth of the point is within range
                w > 0)                          // weight is positive
            {
                posOut += w * vec3(i, j, neighPosTex.z);
                plane += w * neighNormalTex;

                WN += w * neighNormalTex;
                WP += w * relPos;
                WPN += w * mult(relPos, neighNormalTex);
                WPP += w * mult(relPos,  relPos);

                sum += w;
                num ++;
            }
        }
}

void calculateU()
{
    float up = WPN - mult(WP,WN)/sum;
    float tmpDown = mult(WP,WP)/sum;
    float down = WPP - tmpDown;

    u4 = 1*(up / (2*down));
    u1to3 = (WN - 2*u4*WP)/sum;
    u0 = -(mult(u1to3, WP) + u4*WPP)/sum;
}

void pratNorm()
{
    float pn = sqrt(mult(u1to3, u1to3) - 4*u0*u4);
    u4 /= pn;
    u1to3 /= pn;
    u0 /= pn;
}

void converge()
{
    vec3 dir = u1to3 + 2*u4*fit;
    float sz = 1/length(dir);
    dir = dir*sz;
    float ad = u0 + mult(u1to3, fit) + u4 * mult(fit, fit);
    float del = -ad * min(sz, 1);
    fit += dir*del;

    for (int i=1; i<16; ++i)
    {
        vec3 grad = u1to3 + 2.0 * u4 * fit;
        sz = 1/length(grad);
        del = -(u0 + mult(fit,u1to3) + u4*mult(fit,fit)*min(1, sz));
        fit += dir * del;
    }

}

void finilize()
{
    if(showPos) tmpOut = posOut/sum;
    else if (planeFit) num > 0  ? tmpOut = 0.5 * normalize(plane) + vec3(0.5) : vec3(0);
    else if (sum == 0 || num < 6) // if there isn't enough neighbors use the plane fit
        tmpOut = num < 0 ? vec3(0) : 0.5 * normalize(plane) + vec3(0.5);
    else
    {
        calculateU();
        pratNorm();
        converge();
        tmpOut = 0.5 * normalize(u1to3 + 2.0*u4*fit) + vec3(0.5);
    }
}

void main()
{
    init();
    findDepth();
    addNeighbors();
    finilize();

    fragColor = length(tmpOut) > 0 ? vec4(tmpOut, 1.0): vec4(0.2,0.2,0.2,1);
    out_normal = vec4(normalTex, 1.0);
    out_position = vec4(posTex, 1.0);
}
