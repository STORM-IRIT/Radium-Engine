uniform sampler2D desc1;

in vec2 varTexcoord;

layout (location = 0) out vec4 fragColor;

float mtanh(float c) {
  // remapping [-inf,inf] in [-1,1]
  const float tanhmax = 3.11622;
  float x = (c/tanhmax);
  float e = exp(-2.0*x);
  return clamp((1.0-e)/(1.0+e),-1.0,1.0);
}

vec4 curvatureVariation() {
  // |A|B|C|
  // |D|X|E|
  // |F|G|H|

  // differentiate mean curvature
  float A, B, C, D, E, F, G, H;

    A = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2(-1,+1),0).w;
    B = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2( 0,+1),0).w;
    C = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2(+1,+1),0).w;
    D = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2(-1, 0),0).w;
    E = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2(+1, 0),0).w;
    F = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2(-1,-1),0).w;
    G = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2( 0,-1),0).w;
    H = texelFetch(desc1,ivec2(gl_FragCoord.xy)+ivec2(+1,-1),0).w;

  vec2 g = vec2(2.0*E-2.0*D+C-A+H-F,
		2.0*B-2.0*G+A-F+C-H);

  return g==vec2(0.0) ? vec4(0.0) : vec4(normalize(vec2(-g.y,g.x)),mtanh(length(g)),0.0);

}

void main(void) {
  vec4  c = curvatureVariation();

  fragColor = c;
}
