const mat3 RGB2XYZ = mat3(0.5141364,  0.265068, 0.0241188, 0.3238786, 0.67023428, 0.1228178, 0.16036376, 0.06409157, 0.84442666);
const mat3 XYZ2RGB = mat3(2.5651, -1.0217, 0.0753, -1.1665, 1.9777, -0.2543, -0.3986, 0.0439, 1.1892);

float luminance (vec3 color)
{
      return (RGB2XYZ * color).g;
}

// from http://content.gpwiki.org/index.php/D3DBook:High-Dynamic_Range_Rendering
// Convention Yxy --> color.r = Y, color.g=x, color.b=y
vec3 rgb2Yxy(vec3 color){
    vec3 XYZColor = RGB2XYZ*color;
    float num = dot(vec3(1.0,1.0,1.0), XYZColor);
    return vec3(XYZColor.g, XYZColor.rg/num);
}

vec3 Yxy2rgb(vec3 colorYxy){
    float num = colorYxy.r/colorYxy.b;
    vec3 XYZColor = vec3(colorYxy.g*num,
                         colorYxy.r,
                         (1-colorYxy.g-colorYxy.b)*num);
    return XYZ2RGB*XYZColor;
}

float getMiddleGrey(float mean)
{
    return (1.8 - 2.0 / (2.0 + (log(mean + 1) / log(10.0))));
}

float getLumScaled(float Y, float mean, float grey)
{
    return (Y * grey / (mean + 0.001));
}
