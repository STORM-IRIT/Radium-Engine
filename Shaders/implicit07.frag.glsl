uniform sampler2D grad;
uniform sampler2D shading;
uniform sampler2D desc1;
uniform sampler2D desc2;
uniform sampler2D analysis1;
uniform sampler2D analysis2;
uniform sampler2D analysis3;
uniform sampler2D analysis4;
uniform sampler2D analysis5;
uniform sampler2D noise;
uniform sampler2D sing;
int       display;

layout(location = 0) out vec4 fragColor;

vec2 getFittedPointCoord(in vec2 c) {
  const int nbiter = 1;

  vec2 fc = c;//texelFetch(analysis1,c).xy;
  vec2 test;

  for(int i=0;i<nbiter;++i) {
    test = texelFetch(analysis1,ivec2(fc),0).xy;
    fc = test;
  }

  return fc;
}

vec3 hsvToRgb(in float h,in float s,in float v) {
  vec3 rgb;
  int hi = int(floor(h/60.0))%6;
  float f = h/60.0 - floor(h/60.0);
  float p = v*(1.0-s);
  float q = v*(1.0-f*s);
  float t = v*(1.0-(1.0-f)*s);

  if(hi==0) rgb = vec3(v,t,p);
  else if(hi==1) rgb = vec3(q,v,p);
  else if(hi==2) rgb = vec3(p,v,t);
  else if(hi==3) rgb = vec3(p,q,v);
  else if(hi==4) rgb = vec3(t,p,v);
  else rgb = vec3(v,p,q);

  return rgb;
}

vec4 HSVgradient(in sampler2D g) {

  vec3 n = -texelFetch(g,ivec2(gl_FragCoord.st),0).xyz;
  n.z = 1.0-length(n.xy);
  n = normalize(n);//*0.5 + 0.5;


  return vec4(n,1.0);

  const float pi = 3.141592;

  vec4 d = texelFetch(g,ivec2(gl_FragCoord.st),0);

  float gx = d.x;
  float gy = d.y;
  float ma = d.xy==vec2(0.0) ? 0.0 : length(d.xy);

  gx /= ma;
  gy /= ma;
  ma  = tanh(ma);

  float h = gy>0.0 ? acos(gx) : 2.0*pi-acos(gx);
  h = h/(2*pi)*360.0;
  float s = ma;
  float v = 1.0;

  return vec4(hsvToRgb(h,s,v),1.0);
}

vec4 HSVintensity(in sampler2D desc) {
  //return vec4(texelFetch(desc,gl_FragCoord.st).z);
  return vec4(abs(texelFetch(desc,ivec2(gl_FragCoord.st), 0).z));

  vec4  d = texelFetch(desc,ivec2(gl_FragCoord.st),0);
  float h = d.z;

  float caL = 180.0;
  float caR = 250.0;
  float cvL = 40.0;
  float cvR = 0.0;
  float s0  = 0.75;

  float H,S,V;
  H = h<0.0 ? mix(caL,caR,-h) : mix(cvL,cvR,h);
  S = s0;
  V = 1.0;

  vec3 rgb = hsvToRgb(H,S,V);

  return vec4(rgb,1.0);
}

vec4 lic(in sampler2D desc,in bool invertdir) {
  const int halfsize = 10;

  vec2 coord;
  vec2 dir = texelFetch(desc,ivec2(gl_FragCoord.st),0).xy;
  //return vec4(dir, 0,1);
  //vec2 dir = texelFetch(desc,ivec2(gl_FragCoord.st*1.0/vec2(sw,sh)),0).xy;

  if(invertdir)
    dir = vec2(dir.y,-dir.x);

  vec4 res = texelFetch(noise,ivec2(gl_FragCoord.st),0);
  vec2 currentdir;
  vec2 tmpdir;

  coord = gl_FragCoord.st;
  currentdir = dir;
  for(int i=1;i<=halfsize;i++) {
    coord  = coord+vec2(currentdir.x,currentdir.y);
    res   += texelFetch(noise,ivec2(coord),0);

    tmpdir = texelFetch(desc,ivec2(coord),0).xy;
    //tmpdir = texelFetch(desc,ivec2(coord*1.0/vec2(sw,sh)),0).xy;

    if(invertdir)
      tmpdir = vec2(tmpdir.y,-tmpdir.x);

    currentdir = tmpdir;
  }

  coord = gl_FragCoord.st;
  currentdir = dir;
  for(int i=1;i<=halfsize;i++) {
    coord  = coord-vec2(currentdir.x,currentdir.y);
    res   += texelFetch(noise,ivec2(coord),0);

    tmpdir = texelFetch(desc,ivec2(coord),0).xy;
    //tmpdir = texelFetch(desc,ivec2(coord*1.0/vec2(sw,sh)),0).xy;

    if(invertdir)
      tmpdir = vec2(tmpdir.y,-tmpdir.x);

    currentdir = tmpdir;
  }

  res = res/(2.0f*float(halfsize)+1.0f);

  return vec4(res.x);
}

vec4 singularities() {
  return texelFetch(sing,ivec2(gl_FragCoord.st),0);
}


vec4 directions(in sampler2D desc,bool invertdir) {
  //return vec4(abs(texelFetch(analysis2,gl_FragCoord.st).xy),vec2(0.0));
  //return vec4(texelFetch(desc,gl_FragCoord.st).xy,vec2(0.0));

  vec4 d = HSVintensity(desc);
  //return mix(lic(desc,invertdir),d,0.4);

  if(texelFetch(desc,ivec2(gl_FragCoord.st),0).xy==vec2(0.0))
    return vec4(1.0);

  return mix(lic(desc,invertdir),vec4(singularities().x,0.0,0.0,1.0),0.6);
}

vec4 displayTex(in sampler2D tex,bool flter) {
  vec4  v = vec4(texelFetch(tex,ivec2(gl_FragCoord.st),0));
  float f = texelFetch(analysis4,ivec2(gl_FragCoord.st),0).x;

  if(flter)
    return v*f;

  return v;
}

vec4 displayFeatureCurvature() {
  float v = abs(texelFetch(analysis2,ivec2(gl_FragCoord.st),0).z);
  vec4  c = texelFetch(analysis4,ivec2(gl_FragCoord.st),0);

/*   if(v*c.x<0.05) */
/*     return vec4(0.0); */

/*   return vec4(1.0); */

  return vec4(50.0*c.z*c.x);

  float tkx = v*abs(c.y)*10;
  return vec4(tkx);
}

vec4 displayLines() {

  vec4 a1 = texelFetch(analysis1,ivec2(gl_FragCoord.st),0);
  vec4 a4 = texelFetch(analysis4,ivec2(gl_FragCoord.st),0);
  vec4 col= texelFetch(shading,  ivec2(gl_FragCoord.st),0);

  float dmax = 2.0;

  // discarding far points from the top of the maxima
  float dx = 1.0-clamp(abs(a1.z)/dmax,0.0,1.0);

  // discarding filtered lines
  float df = a4.x*2.0*abs(texelFetch(analysis2,ivec2(gl_FragCoord.st),0).z);

  vec4 res = vec4(1.0-dx*df);
  //return mix(vec4(res),vec4(singularities().x,0.0,0.0,1.0),0.6);

  return vec4(1.0-a4.x);

  return res;
}

float fittingStep(in vec2 dir) {
  const int n = 7;
  const int hlf = n/2;

  vec2  ndir  = vec2(dir.x,dir.y);
  vec2  c1    = gl_FragCoord.st;
  vec2  c2    = gl_FragCoord.st;
  float max1  = texelFetch(sing,ivec2(c1),0).x;
  float max2  = max1;
  float total = max1;
  float sing1;
  float sing2;

  for(int i=1;i<=hlf;++i) {

    c1 += ndir;
    c2 -= ndir;

    sing1 = texelFetch(sing,ivec2(c1),0).x;
    sing2 = texelFetch(sing,ivec2(c2),0).x;

    max1 = max(max1,sing1);
    max2 = max(max2,sing2);

    total += max1+max2;
  }

  return smoothstep(0.5,0.7,1.0-min(total/float(n),1.0));
}

vec4 profileIntensity() {
  //vec2 fc = getFittedPointCoord(gl_FragCoord.st);
  //float g = abs(texelFetch(analysis2,fc).w);
  float g = abs(texelFetch(analysis2,ivec2(gl_FragCoord.st),0).z);

  //return -20*vec4(g);//*texelFetch(analysis4,gl_FragCoord.st).x;
  return 1.0*vec4(g)*texelFetch(analysis4,ivec2(gl_FragCoord.st),0).x;
}

vec4 profileExtrema() {
  //vec2 fc = getFittedPointCoord(gl_FragCoord.st);
  //vec4 a1 = texelFetch(analysis1,fc);
  vec4 a1 = texelFetch(analysis1, ivec2(gl_FragCoord.st),0);
  vec4 a4 = texelFetch(analysis4, ivec2(gl_FragCoord.st),0);

  float dmax1 = 4;
  float dmax2 = 1.2;

  // discarding far points from the top of the maxima
  float dx1 = 1.0-clamp(a1.z/dmax1,0.0,1.0);
  float dx2 = 1.0-clamp(a1.z/dmax2,0.0,1.0);
  vec4 val = vec4(a4.x);
  //vec4 val = profileIntensity();

  vec4 colMin  = vec4(1,1,0,1);
  vec4 colMax  = vec4(1.0,0,0.5,1);
  vec4 colSkel = vec4(0.4,0.4,1,1);

  vec4 col1 = mix(colMin,colMax,dx1);
  vec4 col2 = mix(col1,colSkel,dx2);


  if(texelFetch(desc2, ivec2(gl_FragCoord.st),0).xy==vec2(0.0))
    return vec4(1.0);

  return col2*val*1;
}


vec4 skeletonIntensity() {
  vec4 a4 = texelFetch(analysis4, ivec2(gl_FragCoord.st),0);

  return vec4(a4.xyz*1,1.0);
  return vec4(a4.x*a4.y);
}

vec4 skeletonExtrema() {


  vec4 a4 = texelFetch(analysis4, ivec2(gl_FragCoord.st),0);

  return vec4(a4.x*a4.y);
}


void main(void) {
    display = 4 ;
  if(display==0 || display==1) {
      fragColor = displayLines();
  } else if(display==3) {
    fragColor = HSVintensity(desc1)*1;
  } else if(display==3) {
    fragColor = HSVintensity(desc2)*1;
  } else if(display==4) {
    //fragColor = directions(desc2,false);
    fragColor = directions(analysis2,false);
  } else if(display==5) {
    fragColor = directions(desc2,true);
  } else if(display==6) {
    fragColor = displayTex(analysis1,true);
  } else if(display==7) {
    fragColor = displayTex(analysis2,false);
  } else if(display==8) {
    fragColor = displayTex(analysis3,false);
    //fragColor = vec4(texelFetch(analysis4,gl_FragCoord.st).z);
  } else if(display==9) {
    //fragColor = displayTex(analysis4,false);
    fragColor = vec4(1.0-texelFetch(analysis4, ivec2(gl_FragCoord.st),0).x);
    //fragColor = vec4(texelFetch(analysis4,gl_FragCoord.st).xyzw);
  } else if(display==10) {
    fragColor = displayTex(analysis5,false);
  } else if(display==11) {
    fragColor = HSVgradient(grad);
    //fragColor = test()*10;
  } else if(display==12) {
    fragColor = displayFeatureCurvature();
  } else if(display==13) {
    fragColor = singularities();
  }


  else if(display==14) {
  if(texelFetch(desc2,ivec2(gl_FragCoord.st),0).xy==vec2(0.0))
      fragColor = vec4(1.0);
  else
      fragColor = vec4(0.0)+HSVintensity(desc2)*(1.0-texelFetch(sing,ivec2(gl_FragCoord.st),0));
  fragColor = vec4(0.0)+HSVintensity(desc2);//*(1.0-texelFetch(sing,gl_FragCoord.st));
  } else if(display==15) {
    fragColor = directions(desc2,true);
  } else if(display==16) {
  if(texelFetch(desc2,ivec2(gl_FragCoord.st),0).xy==vec2(0.0))
    fragColor = vec4(1.0);
else
    fragColor = profileIntensity();
    fragColor = profileIntensity();
  } else if(display==17) {
    fragColor = profileExtrema();
  } else if(display==18) {
    //fragColor = skeletonIntensity();
    fragColor = texelFetch(shading,ivec2(gl_FragCoord.st),0);
  } else if(display==19) {
    //fragColor = skeletonExtrema();
    if(texelFetch(grad,ivec2(gl_FragCoord.st),0).xy==vec2(0.0))
      fragColor = vec4(1.0);
    else

    fragColor = texelFetch(shading,ivec2(gl_FragCoord.st),0);
  }

   // fragColor = texelFetch(noise, ivec2(gl_FragCoord.st),0);
}
