layout (location = 0) out vec4 out_ssao;

uniform sampler2D ao_sampler;
in vec2 varTexcoord;

const int half_width = 2;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ao_sampler, 0));
    float result = 0.0;
    for (int x = -half_width; x < half_width; ++x)
    {
        for (int y = -half_width; y < half_width; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ao_sampler, varTexcoord + offset).r;
        }
    }
    out_ssao = vec4(vec3(result / (4 * half_width * half_width)), 1);
}
