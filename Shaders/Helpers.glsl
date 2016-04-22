vec3 getKd()
{
    if (material.tex.hasKd == 1)
    {
        return vec3(texture(material.tex.kd, fs_in.texcoord.xy));
    }

    return material.kd.xyz;
}
vec3 getKs()
{
    if (material.tex.hasKs == 1)
    {
        return vec3(texture(material.tex.ks, fs_in.texcoord.xy));
    }

    return material.ks.xyz;
}

float getNs()
{
    float ns = material.ns;
    if (material.tex.hasNs == 1)
    {
        ns = texture(material.tex.ns, fs_in.texcoord.xy).r;
    }

    return max(ns, 0.001);
}

vec3 getNormal()
{
    float dir = gl_FrontFacing ? 1.0 : -1.0;
    if (material.tex.hasNormal == 1)
    {
        vec3 n = normalize(vec3(texture(material.tex.normal, fs_in.texcoord.xy)));
        n = n * 2 - 1;
        return dir * n;
    }

    return vec3(dir * normalize(fs_in.normal));
}

bool toDiscard()
{
    if (material.tex.hasAlpha == 1)
    {
        float alpha = texture(material.tex.alpha, fs_in.texcoord.xy).r;
        if (alpha < 0.1)
        {
            return true;
        }
    }
    return false;
}
