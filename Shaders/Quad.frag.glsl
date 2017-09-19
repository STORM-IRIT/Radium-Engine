#include "Structs.glsl"

in vec2 varTexcoord;

uniform bool showPos;
uniform bool planeFit;
uniform int neighSize;
uniform float depthThresh;
uniform int depthCalc;
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D mask;
uniform sampler2D eye;
uniform sampler2D ks;
uniform sampler2D kd;
uniform sampler2D ns;

uniform Light light;

layout (location = 0) out vec4 grad;
layout (location = 1) out vec4 posColor;
layout (location = 2) out vec4 shade;

float INF = 300000, EPS = 0.001;
float maxDis, depth;
float maxBackDep, minBackDep, maxDep, minDep;
vec3 posOut, pos, plane, WP, WN, u1to3, fit, normalOut;
float msk, sum, WPN, WPP, u4, u0;
int num;
vec3 Ks, Kd, Eye;
float Ns;

vec3 blinnPhongInternal(vec3 d, vec3 n)
{
    const float Pi = 3.14159265;
    vec3 direction = normalize(d);
    vec3 normal = normalize(n);

    // http://www.thetenthplanet.de/archives/255 / VortexEngine BlinnPhong
    Kd /= Pi;
    float normalization = ((Ns + 2) * (Ns + 4)) / (8 * Pi * (exp2(-Ns * 0.5) + Ns));
    Ks *= normalization;

    float diffFactor = max(dot(normal, -direction), 0.0);
    vec3 diff = diffFactor * light.color.xyz * Kd;

    vec3 viewDir = normalize(vec3(varTexcoord, depth) - Eye);
    vec3 halfVec = normalize(viewDir + direction);
    float specFactor = pow(max(dot(normal, -halfVec), 0.0), Ns);
    vec3 spec = specFactor * light.color.xyz * Ks;

    return diff + spec ;
}

float weight(float dis, float m_maxDis)
{
    //return 1;
    if (dis >= m_maxDis) return 0;
    float sqrX = dis/m_maxDis;
    sqrX = sqrX*sqrX;
    return pow((1-sqrX),2);
}

void init()
{
    msk = texelFetch(mask, ivec2(gl_FragCoord),0).x;
    maxDis = neighSize;
    maxBackDep = maxDep = -INF;
    minBackDep = minDep = INF;

    fit = normalOut = posOut = plane = WP = WN = u1to3 = vec3(0);
    sum = WPN = WPP = u4 = u0 = 0;
    num = 0;
}

void updateMask()
{
    int sz = 2;
    if (msk == 1)
        for (int i=-sz; i<=sz; i++)
            for (int j=-sz; j<=sz; j++)
                if(length(vec2(i,j)) <= sz && texelFetch(mask, ivec2(gl_FragCoord) + ivec2(i,j), 0).x == 0)
                {
                    msk = 0;
                    return;
                }
}

void findMat()
{
    vec3 tmpKs, tmpKd, tmpEye;
    float tmpNs;
    bool flag;
    tmpEye = texelFetch(eye, ivec2(gl_FragCoord), 0).xyz;
    tmpKs = texelFetch(ks, ivec2(gl_FragCoord), 0).xyz;
    tmpKd = texelFetch(kd, ivec2(gl_FragCoord), 0).xyz;
    tmpNs = texelFetch(ns, ivec2(gl_FragCoord), 0).x;
    flag = length(tmpKd) > 0;
    int sz = 3;
    if (msk == 1)
        for (int i=-sz; i<=sz && !flag; i++)
            for (int j=-sz; j<=sz && !flag; j++)
            {
                tmpEye = texelFetch(eye, ivec2(gl_FragCoord) + ivec2(i,j), 0).xyz;
                tmpKs = texelFetch(ks, ivec2(gl_FragCoord) + ivec2(i,j), 0).xyz;
                tmpKd = texelFetch(kd, ivec2(gl_FragCoord) + ivec2(i,j), 0).xyz;
                tmpNs = texelFetch(ns, ivec2(gl_FragCoord) + ivec2(i,j), 0).x;
                flag = length(tmpKd) > 0;
            }
    Kd = tmpKd;
    Ks = tmpKs;
    Ns = tmpNs;
    Eye = tmpEye;
}

void findDepth()
{
    float neighMsk, dis = 0, avDep = 0, newAvDep = 0, sum = 0;
    vec3 neighPosTex, neighNormalTex;
    int num = 0;

    for (int k=0; k<10 && msk==1; k++)
    {
        for (int i=-neighSize; i<=neighSize; i++)
            for (int j=-neighSize; j<=neighSize; j++)
            {
                neighPosTex = texelFetch(position, ivec2(gl_FragCoord.xy) + ivec2(i,j), 0).xyz;
                neighNormalTex = texelFetch(normal, ivec2(gl_FragCoord.xy) + ivec2(i,j), 0).xyz;
                dis = length(vec2(i,j));
                if (dis <= maxDis && length(neighPosTex) > 0)
                {
                    if (neighNormalTex.z >= 0)
                    {
                        float w = weight(dis, maxDis);
                        if (k == 0)
                        {
                            if (neighPosTex.z > maxDep) maxDep = neighPosTex.z;
                            if (neighPosTex.z < minDep) minDep = neighPosTex.z;
                        }
                        else w = weight(abs(avDep - neighPosTex.z), maxDep - minDep);//(maxBackDep != -INF && avDep > maxBackDep ? maxBackDep : minDep));
                        newAvDep += neighPosTex.z * w;
                        sum += w;
                    }
                    else
                    {
                        if (neighPosTex.z > maxBackDep) maxBackDep = neighPosTex.z;
                        if (neighPosTex.z < minBackDep) minBackDep = neighPosTex.z;
                    }
                }

            }
        newAvDep/=sum;
        if (abs (newAvDep - avDep) < EPS) break;
        avDep = newAvDep;
        newAvDep = sum = 0;
    }
    switch(depthCalc)
    {
        case 0:
            depth = maxDep;
            break;
        case 1:
            depth = avDep;
            break;
        case 2:
            if (abs (avDep - maxDep) < depthThresh) depth = maxDep;
            else depth = avDep;
            break;
        default:
            break;
    }
}

void addNeighbors()
{
    //float ndepth = (depth - minDep)/(maxDep-minDep);
    pos = vec3(varTexcoord, depth);
    int i, j;
    for (int crcl = 0;  msk == 1 && crcl<=neighSize && num < 6; crcl++)
    for (int pt=-neighSize; pt<neighSize; pt++)
    for (int ind=0; ind<4; ind++)
    {
        switch(ind)
        {
        case 0:
            i = pt;
            j = -crcl;
            break;
        case 1:
            i = crcl;
            j = pt;
            break;
        case 2:
            i = -pt;
            j = crcl;
            break;
        case 3:
            i = -crcl;
            j = -pt;
            break;
        }
        vec3 neighPosTex = texelFetch(position, ivec2(gl_FragCoord.xy) + ivec2(i,j), 0).xyz;
        vec3 neighNormalTex = texelFetch(normal, ivec2(gl_FragCoord.xy) + ivec2(i,j), 0).xyz;
        vec3 relPos = vec3(i, j, depth - neighPosTex.z);

        float dis = length(vec2(i, j));
        float w =  weight(dis, maxDis);//weight(abs(depth - neighPosTex.z), depthThresh);

        if (dis <= maxDis &&                     // point is in range
            neighNormalTex.z >= 0 &&            //point is facing forward
            length(neighPosTex) > 0 &&          // there actually is a point!
            //abs(depth-neighPosTex.z) <= depthThresh &&
            w > 0)                              // weight is positive
        {
            posOut += w * vec3(ivec2(gl_FragCoord.xy) + ivec2(i,j), -neighPosTex.z);
            plane += w * neighNormalTex;

            WN += w * neighNormalTex;
            WP += w * relPos;
            WPN += w * dot(relPos, neighNormalTex);
            WPP += w * dot(relPos,  relPos);

            sum += w;
            num ++;
        }
    }
}

void calculateU()
{
    float up = WPN - dot(WP,WN)/sum;
    float tmpDown = dot(WP,WP)/sum;
    float down = WPP - tmpDown;

    u4 = 1*(up / (2*down));
    u1to3 = (WN - 2*u4*WP)/sum;
    u0 = -(dot(u1to3, WP) + u4*WPP)/sum;
}

void pratNorm()
{
    float pn = sqrt(dot(u1to3, u1to3) - 4*u0*u4);
    u4 /= pn;
    u1to3 /= pn;
    u0 /= pn;
}

void converge()
{
    vec3 dir = u1to3 + 2*u4*fit;
    float sz = 1/length(dir);
    dir = dir*sz;
    float ad = u0 + dot(u1to3, fit) + u4 * dot(fit, fit);
    float del = -ad * min(sz, 1);
    fit.z += (dir*del).z;

    for (int i=1; i<16; ++i)
    {
        vec3 grad = u1to3 + 2.0 * u4 * fit;
        sz = 1/length(grad);
        del = -(u0 + dot(fit,u1to3) + u4*dot(fit,fit)*min(1, sz));
        fit.z +=( grad * del).z;
    }

}

void finilize()
{
    if (planeFit) num > 6 ? normalOut = normalize(plane) : vec3(0);
    else if (sum == 0 || num <= 6) normalOut = vec3(0); // if there isn't enough neighbors ignore the point
    else
    {
        calculateU();
        pratNorm();
        converge();
       // if (num > 25) tmpOut = vec3(0.2);
      //  else
        normalOut = normalize(u1to3 + 2.0*u4*fit) ;
    }
}

void main()
{
    init();
    updateMask();
    findDepth();
    findMat();
    addNeighbors();
    finilize();

    if (msk == 1)
    {
        float tmpDep = -depth;
        float h = u0 + dot(fit, u1to3) + dot(fit, fit)*u4;
        grad = vec4(-normalOut.xy, tmpDep, tmpDep);//vec4(-normalOut.xy, tmpDep, tmpDep);//vec4(1.0-max(dot(normalOut, Eye-vec3(varTexcoord, depth)),0.0),depth, depth, depth);//
        posColor = vec4(varTexcoord, tmpDep, h);
        shade = vec4(blinnPhongInternal(light.directional.direction, normalOut) + Kd * 0.1, 1);
    }
    else shade = posColor = grad = vec4(0,0,0,1.0);
}
