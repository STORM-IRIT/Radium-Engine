out vec4 fragColor;

struct Material
{
    vec4 kd;
};

uniform Material material;

void main()
{
    fragColor = vec4(material.kd.xyz, 1);
}
