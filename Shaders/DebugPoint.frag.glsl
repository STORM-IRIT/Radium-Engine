/* RADIUM_SHADER_GLOBAL_REPLACE_OTHER */

out vec4 fragColor;

in vec3 vCol;

void main()
{
    fragColor = vec4(vCol, 1.0);
}
