uniform sampler2D grad; // gradient tex
uniform float     silhouette; //## min : 0 ## max : 0.2 ## default : 0.03

layout (location = 0) out vec4 fragColor;

float silw(in float s) {
  // silhouette weight
  float t2 = 0.9+silhouette;
  float t1 = t2-0.01;

  return smoothstep(t1,t2,max(1.0-abs(s),0.9));
}

void main(void) {
  //if(X.xyz==vec3(0.0)) {
  //discard;
  //}
  vec4 X = texelFetch(grad,ivec2(gl_FragCoord.st),0);
  vec4 A = texelFetch(grad,ivec2(gl_FragCoord.s-1,gl_FragCoord.t+1),0);
  vec4 B = texelFetch(grad,ivec2(gl_FragCoord.s  ,gl_FragCoord.t+1),0);
  vec4 C = texelFetch(grad,ivec2(gl_FragCoord.s+1,gl_FragCoord.t+1),0);
  vec4 D = texelFetch(grad,ivec2(gl_FragCoord.s-1,gl_FragCoord.t  ),0);
  vec4 E = texelFetch(grad,ivec2(gl_FragCoord.s+1,gl_FragCoord.t  ),0);
  vec4 F = texelFetch(grad,ivec2(gl_FragCoord.s-1,gl_FragCoord.t-1),0);
  vec4 G = texelFetch(grad,ivec2(gl_FragCoord.s  ,gl_FragCoord.t-1),0);
  vec4 H = texelFetch(grad,ivec2(gl_FragCoord.s+1,gl_FragCoord.t-1),0);

  // anisotropic diffusion
  const float lambda = 0.1;
  vec2 g = X.xy + lambda*(silw(B.z-X.z)*(B.xy-X.xy) +
			  silw(G.z-X.z)*(G.xy-X.xy) +
			  silw(D.z-X.z)*(D.xy-X.xy) +
			  silw(E.z-X.z)*(E.xy-X.xy) +
			  0.5*(silw(A.z-X.z)*(A.xy-X.xy) +
			       silw(C.z-X.z)*(C.xy-X.xy) +
			       silw(F.z-X.z)*(F.xy-X.xy) +
			       silw(H.z-X.z)*(H.xy-X.xy)));


  // diffuse depth without insulators
  float d = X.w + lambda*((B.w-X.w) +
			  (G.w-X.w) +
			  (D.w-X.w) +
			  (E.w-X.w) +
			  0.5*((A.w-X.w) +
			       (C.w-X.w) +
			       (F.w-X.w) +
			       (H.w-X.w)));

  fragColor = vec4(g.x,g.y,X.z,d);

}
