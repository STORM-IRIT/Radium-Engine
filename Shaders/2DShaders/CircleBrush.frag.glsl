in vec2 varTexcoord;

out vec4 fragColor;

uniform vec2  mousePosition;
uniform float brushRadius;
uniform vec2  dim;

float width = 1;

void main()
{
    vec2 c = varTexcoord*dim;
    float dist = length(mousePosition-c);
    if(dist>brushRadius+width || dist<brushRadius-width)
        discard;
    else
        fragColor = vec4(1,0.75,0,1);
}


