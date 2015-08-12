out vec4 fragId;

uniform vec4 objectId;

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

    fragId = objectId;
}
