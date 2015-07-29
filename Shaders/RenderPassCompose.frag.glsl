out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;
uniform sampler2D ambient;
uniform sampler2D oitSumColor;
uniform sampler2D oitSumWeight;

uniform int renderpass;

void opaqueCompose()
{
    fragColor = vec4(texture(color, varTexcoord)) + vec4(texture(ambient, varTexcoord));
}

void transparentCompose()
{
    float r = texture(oitSumWeight, varTexcoord).r;
    
    // No transparent stuff here, just move on
    if (r >= 1.0)
    {
        discard;
    }
    
    vec4 accum = vec4(texture(oitSumColor, varTexcoord));
    
    vec3 avgColor = accum.rgb / max(accum.a, 0.00001);
    
    fragColor = vec4(avgColor, r);
}

void main()
{
    switch (renderpass)
    {
        case 0:
        {
            opaqueCompose();
        } break;
        
        case 1:
        {
            transparentCompose();
        } break;
    }
}

