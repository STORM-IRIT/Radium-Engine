uniform sampler2D desc1;
uniform sampler2D desc2;
uniform sampler2D sing;
//uniform int       type;

int type;

// type = 0: nothing 
// type = 1: suggestive contours (SC)
// type = 2: suggestive highlights (SH)
// type = 3: SC + SH
// type = 4: apparent ridges (AR) 
// type = 5: apparent valleys (AV)
// type = 6: AR + AV
// type = 7: demarcating curves 

const int n  = 9; // fitting size 
const int nb = 2; // number of fittings
uniform vec4 H0, H1, H2, H3, H4, H5, H6, H7, H8;
vec4 H[n];

layout (location = 0) out vec4 A1;
layout (location = 1) out vec4 A2;
layout (location = 2) out vec4 A3;

vec4 maxDirectionAndValue(in sampler2D desc) {
  vec4 d = texelFetch(desc,ivec2(gl_FragCoord.xy),0);
  return vec4(d.y,-d.x,d.z,d.w);
}

float mtanh(float c) {
  // remapping [-inf,inf] in [-1,1]
  const float tanhmax = 3.11622;
  float x = (c/tanhmax);
  float e = exp(-2.0*x);
  return clamp((1.0-e)/(1.0+e),-1.0,1.0);
}

float fittingStep(in vec2 dir) {
  const int haf = n/2;

  vec2  ndir   = vec2(dir.x,dir.y);
  vec2  c1     = gl_FragCoord.xy;
  vec2  c2     = gl_FragCoord.xy;
  float max1   = texelFetch(sing,ivec2(c1),0).x;
  float max2   = max1;
  float total1 = max1;
  float total2 = max1;
  float sing1;
  float sing2;

  for(int i=1;i<=haf;++i) {
    
    if(total1>=1 || total2>=1)
      return float(i-1)/float(haf);

    c1 += ndir;
    c2 -= ndir;
    
    sing1  = texelFetch(sing,ivec2(c1),0).x;
    sing2  = texelFetch(sing,ivec2(c2),0).x;
    
    //max1 = max(max1,sing1);
    //max2 = max(max2,sing2);
    
    //total += max1+max2;

    total1 += sing1;
    total2 += sing2;
  }
  
  return 1.0;
  //return 1.0-min(total/float(n),1.0);
}

float cubicValue(in vec4 c,in float x) {
  float x2 = x*x;
  float x3 = x2*x;
  
  return c.x*x3 + c.y*x2 + c.z*x + c.w;
}

vec3 cubicMaxima(in vec4 cubic,in float f,in int t) {
  const float dmax = 10000000.0;
  const float eps  = 0.00000001;

  // searching cubic solutions
  vec3  quadric = vec3(3.0*cubic.x,2.0*cubic.y,cubic.z);
  float disc    = quadric.y*quadric.y-4.0*quadric.x*quadric.z;

  if(disc<=0.0 || abs(quadric.x)<eps) {
    return vec3(dmax,0.0,0.0);
  }

  
  float x1 = (-quadric.y-sqrt(disc))/(2.0*quadric.x);
  float x2 = quadric.z/(quadric.x*x1);
  
  // searching best solution (depending on the type of the line)
  float t1 = -(6.0*cubic.x*x1 + 2.0*cubic.y);
  float t2 = -(6.0*cubic.x*x2 + 2.0*cubic.y);

  float c1 = abs(2.0*cubic.y)*sign(t1);
  float c2 = abs(2.0*cubic.y)*sign(t2);

  float v1 = cubicValue(cubic,x1);
  float v2 = cubicValue(cubic,x2);

  return abs(v1)<abs(v2) ? vec3(x1*f,c1*f*f,v1) : vec3(x2*f,c2*f*f,v2);
  //return abs(x1)<abs(x2) ? vec3(x1*f,c1*f*f,v1) : vec3(x2*f,c2*f*f,v2);

 /* if(t==0 || t==3 || t==6 || t==7) {
    // R/V, SC/SH or demarcating curves
    return abs(x1)<abs(x2) ? vec3(x1*f,c1*f*f,v1) : vec3(x2*f,c2*f*f,v2);
  }

  if(t==0 || t==1 || t==5) {
  */  // looking for convexities
    return c1>=0.0 ? vec3(x1*f,c1*f*f,v1) : vec3(x2/f,c2*f*f,v2);
 /* }

  if(t==2 || t==4) {
    // looking for concavities
    return c1<0.0 ? vec3(x1*f,c1*f*f,v1) : vec3(x2*f,c2*f*f,v2);
  }*/
}

vec3 fitCubic(in vec2 dir,in sampler2D desc,float f,int t) {
  const int haf = n/2;
  
  // computing cubic paramaters
  float curv;
  vec4  cubic = vec4(0.0);
  vec2  ndir  = vec2(dir.x,dir.y)*f;
  vec2  coord = vec2(gl_FragCoord.xy-float(haf)*ndir);

  for(int i=0;i<n;++i) {
    curv   = texelFetch(desc,ivec2(coord), 0).z;
    cubic += H[i]*curv;
    coord += ndir;
  }

  return cubicMaxima(cubic,f,t);
}

vec3 fitQuadric(in vec2 dir,in sampler2D desc,float f) {
  const int haf = n/2;

  // computing quadric paramaters   
  float curvature;
  vec3  quadric = vec3(0.0);
  vec2  ndir    = vec2(dir.x,dir.y)*f;
  vec2  coord   = vec2(gl_FragCoord.xy-float(haf)*ndir);

  // matrix multiplication 
  for(int i=0;i<n;++i) {
    curvature  = texelFetch(desc,ivec2(coord),0).z;
    quadric   += H[i].xyz*curvature;    
    coord     += ndir;
  }
  
  // singularities 
  if(quadric.x==0.0) {
    return vec3(0.0);
  }

  // searching quadric solution 
  float x = -quadric.y/(2.0*quadric.x);
  float c = -quadric.x;

  // searching quadric value
  float x2 = x*x;
  float s  = quadric.x*x2 + quadric.y*x + quadric.z;

  return vec3(x*f,c*f*f,s);
}

vec2 coordOfPointP(in vec2 dir, in float dist,int t) {
  // in texture coordinates... 
  if(t==0) {
    const float offset = 0.0;
    vec2 d = dir;
    vec2 c = gl_FragCoord.xy + (dist+offset)*normalize(d);
    return c;
  } 
  
  return gl_FragCoord.xy + dir*dist*vec2(1,1);
}

vec3 tangentOfPointP(in vec2 coord,in sampler2D desc) {
  //return texelFetch(desc,coord,0).xyz;
  return texelFetch(desc,ivec2(coord/vec2(1,1)),0).xyz;
}

float validity(in vec4 a1,in vec4 a2,in vec4 a3,float f,int t) {
  const float s1 = 0.0;
  const float s2 = 0.01;

  // discarding inverted convexity
  float dt = 1.0;
  float dr = 1.0;
  float dx = 1.0;
  float df = 1.0;
  float dd = 1.0;
  float dp = 1.0;

  // applying specific filters
  if(t==1) {
    // suggestive contours
    dt = (a2.z<0.0 && a2.w>0.0) || (a2.z>0.0 && a2.w<0.0) ? 0.0 : 1.0;

  } else if(t==2) {
    // suggestive highlights
    dt = (a2.z<0.0 && a2.w>0.0) || (a2.z>0.0 && a2.w<0.0) ? 1.0 : 0.0;

  } else if(t==4) {
    // apparent ridges
    vec2 k = a3.z>=a3.w ? vec2(a3.z,a3.w) : vec2(a3.w,a3.z);
    dr = smoothstep(s1,s2,-k.y-abs(k.x));
    
    dr *= smoothstep(0.0,0.005,-a2.w);

  } else if(t==5) {
    // apparent valleys
    vec2 k = a3.z>=a3.w ? vec2(a3.z,a3.w) : vec2(a3.w,a3.z);
    dr = smoothstep(s1,s2,-abs(k.y)+k.x);

    dr *= smoothstep(0.0,0.005,a2.w);

  } else if(t==6) {
    // AR + AH
    vec2 k = a3.z>=a3.w ? vec2(a3.z,a3.w) : vec2(a3.w,a3.z);
    if(k.y<-abs(k.x)) {
      dr = smoothstep(s1,s2,-k.y-abs(k.x));
    } else if(k.x>abs(k.y)) {
      dr = smoothstep(s1,s2,-abs(k.y)+k.x);
    } else dr = 0.0;
  
  } else if(t==7) {
    // demarcating curves 
    dt = (a2.z<0.0 && a2.w>0.0) || (a2.z>0.0 && a2.w<0.0) ? 0.0 : 1.0;

  } else if(t==0) {
    // silhouettes
    //dr = 0.0; // do not display them
  }

  // global filter on distance max
  dd = 1.0-smoothstep(0.0,10.0,a1.z);
  
  // global filter on plane surfaces 
  dp = (a2.xy==vec2(0.0) || a3.xy==vec2(0.0)) ? 0.0 : 1.0;

  if(t!=0) {
    // global filter on fitting error and 
    df = 1.0-smoothstep(0.1,0.2,abs(a2.z-texelFetch(desc2,ivec2(a1.xy),0).z));
    dx = smoothstep(0.0,0.05,f);
  } else {
    df = 1.0;//-smoothstep(0.1,0.2,abs(a2.z-texelFetch(desc1,a1.xy,0).z));
  }

  //return 1.0-smoothstep(0.1,0.2,abs(a2.z-texelFetch(desc2,a1.xy,0).z));
  return dt*dx*dr*df*dd*dp;
}

bool isSilhouette(in vec4 dv) {
  //return true;
  return dv.z>0.05;
}

void main(void) {
  vec4  a1,a2,a3;
  vec3  fitval,tangent;
  vec2  coord;

  H[0] = H0;
  H[1] = H1;
  H[2] = H2;
  H[3] = H3;
  H[4] = H4;
  H[5] = H5;
  H[6] = H6;
  H[7] = H7;
  H[8] = H8;
  type = 1;
  a3 = maxDirectionAndValue(desc1);

  if(isSilhouette(a3)) {
    // silhouettes 
    //fitval = fitQuadric(a3.xy,desc1,1.0);
    fitval = fitCubic(a3.xy,desc1,1.0,0);
    coord  = coordOfPointP(a3.xy,fitval.x,0);
    tangent = tangentOfPointP(gl_FragCoord.xy,desc1);
    //tangent = tangentOfPointP(coord,desc1);

    a1 = vec4(coord,abs(fitval.x),0.0);
    a2 = vec4(tangent.xy,fitval.z,fitval.y);
    a1.w = -validity(a1,a2,a3,0.0,0); // negative validity for silhouettes
    
    A1 = a1;
    A2 = a2;
    A3 = a3;
    //A3 = vec4(1);
    
  } else {
    // compute user selected line data  
    a3      = maxDirectionAndValue(desc2);
    float f = fittingStep(a3.xy);
    //fitval  = fitQuadric(a3.xy,desc2,f);
    fitval  = fitCubic(a3.xy,desc2,f,type);
    coord   = coordOfPointP(a3.xy,fitval.x,type);
    tangent = tangentOfPointP(gl_FragCoord.xy,desc2);
    //tangent = tangentOfPointP(coord,desc2); 
    
    a1 = vec4(coord,abs(fitval.x),float(type));

    a2 = vec4(tangent.xy,fitval.z,fitval.y);
    a1.w = validity(a1,a2,a3,f,type);

    A1 = a1;
    A2 = a2;
    A3 = a3;
  }

   //A1 = vec4(vec3(1-mtanh(length(gl_FragCoord.xy-coord))), mtanh(abs(fitval.x)));
    //A1 = vec4(vec3(a1.w), 1);
}
