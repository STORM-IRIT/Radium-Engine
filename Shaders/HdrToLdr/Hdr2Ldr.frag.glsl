out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D screenTexture;
uniform float gamma;
void main()
{
    vec2 size = vec2(textureSize(screenTexture, 0));
    vec4 pixel = vec4(texelFetch(screenTexture, ivec2(varTexcoord.xy * size), 0));
    if (pixel.a != 0)
        pixel.rgb=pow(pixel.rgb, vec3(1/gamma));
    fragColor = pixel;
}
