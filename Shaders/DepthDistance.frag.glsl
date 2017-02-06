out vec4 fragColor;

in float far;
in float near;

float LinearizeDepth(float depth, float near, float far) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near) / (far + near - z * (far - near));	
}

void main()
{
    // linear depth
    float depth = LinearizeDepth(gl_FragCoord.z, 0.1, 6.0);
    fragColor = vec4(depth, depth, depth, 1.0f);
}
