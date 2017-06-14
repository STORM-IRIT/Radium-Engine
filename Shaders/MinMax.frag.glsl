out vec4 fragColor;

uniform sampler2D color;

void main()
{
    ivec2 uv0 = ivec2((gl_FragCoord.xy - vec2(.5)) * 2.0);
    ivec2 uv1 = ivec2((gl_FragCoord.xy - vec2(.5)) * 2.0) + ivec2(1, 0);
    ivec2 uv2 = ivec2((gl_FragCoord.xy - vec2(.5)) * 2.0) + ivec2(1, 1);
    ivec2 uv3 = ivec2((gl_FragCoord.xy - vec2(.5)) * 2.0) + ivec2(0, 1);

    vec4 p0 = texelFetch(color, uv0, 0);
    vec4 p1 = texelFetch(color, uv1, 0);
    vec4 p2 = texelFetch(color, uv2, 0);
    vec4 p3 = texelFetch(color, uv3, 0);

    float logs = p0.z * p0.w + p1.z * p1.w + p2.z * p2.w + p3.z * p3.w;
    float weights = (p0.w + p1.w + p2.w + p3.w) * 0.25;
    float minv = min(min(p0.x, p1.x), min(p2.x, p3.x));
    float maxv = max(max(p0.y, p1.y), max(p2.y, p3.y));

    if (isnan(logs))
    {
        logs = 0.0;
    }

    fragColor = vec4(minv, maxv, logs, weights);
//    fragColor = vec4(p0);
}
