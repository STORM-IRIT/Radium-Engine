out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D screenTexture;
uniform int isDepthTexture;
uniform float zNear;
uniform float zFar;

uniform float totalTime;

void main()
{
    vec4 value;

    value = vec4(texture(screenTexture, varTexcoord));

    if (bool(isDepthTexture))
    {
        float n = 1.0;
        float f = 1000.0;
        float z = value.r;
        float linearDepth = (2.0 * n) / (f + n - z  * (f - n));
        value.rgb = vec3(z);
    }

//    fragColor = value;
    float c = abs(sin(totalTime));
    fragColor = vec4(c, 1.0 - c, 0, 1);
//    fragColor = vec4(vTexcoord, 0, 1);
}

