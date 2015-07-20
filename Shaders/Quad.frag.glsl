out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D depth;
uniform sampler2D color;
uniform int isDepthTexture;
uniform float zNear;
uniform float zFar;

void main()
{
    vec4 value;
    int d = 0;

    if (d == 1)
    {
        value = vec4(texture(depth, varTexcoord));

        float n = 1.0;
        float f = 1000.0;
        float z = value.r;
        float linearDepth = (2.0 * n) / (f + n - z  * (f - n));
        value.rgb = vec3(z);
    }
    else
    {
        value = vec4(texture(color, varTexcoord));
    }

    fragColor = value;
//    fragColor = vec4(varTexcoord, 0, 1);
}

