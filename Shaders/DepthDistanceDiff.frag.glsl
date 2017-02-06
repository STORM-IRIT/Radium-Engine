uniform sampler2D u_depthDistance0;
uniform sampler2D u_depthDistance1;

in vec2 varTexcoord;
out vec4 fragColor;

void main ()
{
    fragColor = texture(u_depthDistance1, varTexcoord) - texture(u_depthDistance0, varTexcoord);
    //fragColor = texture(u_depthDistance1, gl_FragCoord.xy) - texture(u_depthDistance0, gl_FragCoord.xy);
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    fragColor.a = 1.0;
}
