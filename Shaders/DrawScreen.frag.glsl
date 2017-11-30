out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D screenTexture;

void main()
{
    vec2 size = vec2(textureSize(screenTexture, 0));
    fragColor = vec4(texelFetch(screenTexture, ivec2(varTexcoord.xy * size), 0));
}
