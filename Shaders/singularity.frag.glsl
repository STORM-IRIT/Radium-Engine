uniform sampler2D grad;
uniform sampler2D desc2;

layout (location = 0) out vec4 fragColor;

void main(void) {
  vec4 vals[9];
  // |v[0]|v[1]|v[2]|
  // |v[3]|v[8]|v[4]|
  // |v[5]|v[6]|v[7]|


  vals[0] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2(-1,+1),0);
  vals[1] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2( 0,+1),0);
  vals[2] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2(+1,+1),0);
  vals[3] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2(-1, 0),0);
  vals[4] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2(+1, 0),0);
  vals[5] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2(-1,-1),0);
  vals[6] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2( 0,-1),0);
  vals[7] = texelFetch(desc2,ivec2(gl_FragCoord.xy)+ivec2(+1,-1),0);
  vals[8] = texelFetch(desc2,ivec2(gl_FragCoord.xy),0);


  float sing1 = 1.0-(abs(dot(vals[8].xy,vals[0].xy)) +
		     abs(dot(vals[8].xy,vals[1].xy)) +
		     abs(dot(vals[8].xy,vals[2].xy)) +
		     abs(dot(vals[8].xy,vals[3].xy)) +
		     abs(dot(vals[8].xy,vals[4].xy)) +
		     abs(dot(vals[8].xy,vals[5].xy)) +
		     abs(dot(vals[8].xy,vals[6].xy)) +
		     abs(dot(vals[8].xy,vals[7].xy)))/8.0;

/*   float sing1 = 1.0-(abs(dot(vals[8].xy,vals[0].xy)) + */
/* 		     abs(dot(vals[8].xy,vals[1].xy)) + */
/* 		     abs(dot(vals[8].xy,vals[2].xy)) + */
/* 		     abs(dot(vals[8].xy,vals[3].xy)) + */
/* 		     abs(dot(vals[8].xy,vals[4].xy)) + */
/* 		     abs(dot(vals[8].xy,vals[5].xy)) + */
/* 		     abs(dot(vals[8].xy,vals[6].xy)) + */
/* 		     abs(dot(vals[8].xy,vals[7].xy)) + */

/* 		     abs(dot(vals[7].xy,vals[0].xy)) + */
/* 		     abs(dot(vals[7].xy,vals[1].xy)) + */
/* 		     abs(dot(vals[7].xy,vals[2].xy)) + */
/* 		     abs(dot(vals[7].xy,vals[3].xy)) + */
/* 		     abs(dot(vals[7].xy,vals[4].xy)) + */
/* 		     abs(dot(vals[7].xy,vals[5].xy)) + */
/* 		     abs(dot(vals[7].xy,vals[6].xy)) + */

/* 		     abs(dot(vals[6].xy,vals[0].xy)) + */
/* 		     abs(dot(vals[6].xy,vals[1].xy)) + */
/* 		     abs(dot(vals[6].xy,vals[2].xy)) + */
/* 		     abs(dot(vals[6].xy,vals[3].xy)) + */
/* 		     abs(dot(vals[6].xy,vals[4].xy)) + */
/* 		     abs(dot(vals[6].xy,vals[5].xy)) + */

/* 		     abs(dot(vals[5].xy,vals[0].xy)) + */
/* 		     abs(dot(vals[5].xy,vals[1].xy)) + */
/* 		     abs(dot(vals[5].xy,vals[2].xy)) + */
/* 		     abs(dot(vals[5].xy,vals[3].xy)) + */
/* 		     abs(dot(vals[5].xy,vals[4].xy)) + */

/* 		     abs(dot(vals[4].xy,vals[0].xy)) + */
/* 		     abs(dot(vals[4].xy,vals[1].xy)) + */
/* 		     abs(dot(vals[4].xy,vals[2].xy)) + */
/* 		     abs(dot(vals[4].xy,vals[3].xy)) + */

/* 		     abs(dot(vals[3].xy,vals[0].xy)) + */
/* 		     abs(dot(vals[3].xy,vals[1].xy)) + */
/* 		     abs(dot(vals[3].xy,vals[2].xy)) + */

/* 		     abs(dot(vals[2].xy,vals[0].xy)) + */
/* 		     abs(dot(vals[2].xy,vals[1].xy)) + */

/* 		     abs(dot(vals[1].xy,vals[0].xy)))/36.0; */



/*   float sing1 = 1.0-(abs(dot(vals[8].xy,vals[0].xy)) * */
/* 		     abs(dot(vals[8].xy,vals[1].xy)) * */
/* 		     abs(dot(vals[8].xy,vals[2].xy)) * */
/* 		     abs(dot(vals[8].xy,vals[3].xy)) * */
/* 		     abs(dot(vals[8].xy,vals[4].xy)) * */
/* 		     abs(dot(vals[8].xy,vals[5].xy)) * */
/* 		     abs(dot(vals[8].xy,vals[6].xy)) * */
/* 		     abs(dot(vals[8].xy,vals[7].xy))); */

/*   float sing1 = 1.0-(min(abs(dot(vals[8].xy,vals[0].xy)), */
/* 			 min(abs(dot(vals[8].xy,vals[1].xy)), */
/* 			     min(abs(dot(vals[8].xy,vals[2].xy)), */
/* 				 min(abs(dot(vals[8].xy,vals[3].xy)), */
/* 				     min(abs(dot(vals[8].xy,vals[4].xy)), */
/* 					 min(abs(dot(vals[8].xy,vals[5].xy)), */
/* 					     min(abs(dot(vals[8].xy,vals[6].xy)), */
/* 						 abs(dot(vals[8].xy,vals[7].xy)))))))))); */


  vals[0] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2(-1,+1),0);
  vals[1] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2( 0,+1),0); // texelFetch(grad, ivec2(gl_FragCoord.xy)+ivec2(1,1), 0)
  vals[2] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2(+1,+1),0);
  vals[3] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2(-1, 0),0);
  vals[4] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2(+1, 0),0);
  vals[5] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2(-1,-1),0);
  vals[6] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2( 0,-1),0);
  vals[7] = texelFetch(grad,ivec2(gl_FragCoord.xy)+ivec2(+1,-1),0);
  vals[8] = texelFetch(grad,ivec2(gl_FragCoord.xy),0);


  vec2 g = vec2(2.0*vals[4].z-2.0*vals[3].z+vals[2].z-vals[0].z+vals[7].z-vals[5].z,
		2.0*vals[1].z-2.0*vals[6].z+vals[0].z-vals[5].z+vals[2].z-vals[7].z);

  float sing2 = tanh(length(g));

/*   float sing2 = (abs(vals[8].z-vals[0].z) + */
/* 		 abs(vals[8].z-vals[1].z) + */
/* 		 abs(vals[8].z-vals[2].z) + */
/* 		 abs(vals[8].z-vals[3].z) + */
/* 		 abs(vals[8].z-vals[4].z) + */
/* 		 abs(vals[8].z-vals[5].z) + */
/* 		 abs(vals[8].z-vals[6].z) + */
/* 		 abs(vals[8].z-vals[7].z))/8.0; */

  float sing = max(smoothstep(0.0,0.5,sing1),
		   smoothstep(0.0,0.5,sing2));

  fragColor = vec4(sing);
}

