out ivec4 fragId;

uniform int objectId;
uniform int drawFixedSize;

void main()
{
    // FIXME(Charly): Do we want to check alpha value of objects to perform picking ?
    /*
    if (material.tex.hasAlpha == 1)
    {
        float alpha = texture(material.tex.alpha, vTexcoord.xy).r;
        if (alpha < 0.1)
        {
            discard;
        }
    }
        */

    fragId.r = objectId;
}
