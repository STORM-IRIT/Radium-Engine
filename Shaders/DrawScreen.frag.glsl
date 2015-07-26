out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D screenTexture;
uniform sampler2D depth;
uniform sampler2D ambient;
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D color;

uniform int isDepthTexture;
uniform float zNear;
uniform float zFar;

uniform float totalTime;

void main()
{
    vec4 value = vec4(texture(screenTexture, varTexcoord));

    vec4 depthValue     = vec4(texture(depth    , varTexcoord));
    vec4 ambientValue   = vec4(texture(ambient  , varTexcoord));
    vec4 positionValue  = vec4(texture(position , varTexcoord));
    vec4 normalValue    = vec4(texture(normal   , varTexcoord));
    vec4 colorValue     = vec4(texture(color    , varTexcoord));

    // depth -> linearize
    float n = 1.0;
    float f = 1000.0;
    float z = depthValue.r;
    float linearDepth = (2.0 * n) / (f + n - z  * (f - n));
    depthValue = vec4(vec3(linearDepth), 1.0);

//    positionValue = positionValue.xyz != vec3(0) ? positionValue / 2 + 0.5 : vec4(0);
    normalValue = normalValue.xyz != vec3(0) ? normalValue / 2 + 0.5 : vec4(0);

//    value = normalValue;

    fragColor = value;
}

