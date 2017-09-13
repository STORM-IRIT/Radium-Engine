uniform sampler2D analysis1;
uniform sampler2D analysis2;
uniform sampler2D analysis3;
uniform sampler2D desc1;
uniform sampler2D desc2;
uniform sampler2D grad;
uniform sampler2D shading;
uniform sampler2D sing;
uniform float     sw;
uniform float     sh;

layout (location = 0) out vec4 fragColor;
// uniform float silhouettes; //## min : 0 ## max : 5.0 ## default : 0.1
// uniform float lines; //## min : 0 ## max : 1.0 ## default : 0.01
// uniform float RAndV; //## min : 0 ## max : 1 ## default : 0.13
// uniform float ff; //## min : 0.001 ## max : 1 ## default : 0
// uniform float fs; //## min : 0 ## max : 1 ## default : 0.01

//uniform float silhouettes; //## min : 0 ## max : 5.0 ## default : 0.0
//uniform float lines; //## min : 0 ## max : 1.0 ## default : 0.0
//uniform float RAndV; //## min : 0 ## max : 1 ## default : 0.0
//uniform float ff; //## min : 0.0 ## max : 1 ## default : 0
//uniform float fs; //## min : 0 ## max : 1 ## default : 0.0

const int fitsize = 9;
uniform vec4 H[fitsize];

bool testVal(in float val) {
  const float eps = 0.0001;

  //return isnan(val) || isinf(val);
  //return isnan(val) || isinf(val) || val==0.0 || abs(val)<eps;

  return val==0.0;//val<0.99999;
}

bool testDir(in vec2 dir) {
  return testVal(length(dir));
  //return testVal(dir.x) || testVal(dir.y) || testVal(length(dir));
}

vec4 getTex(in sampler2D tex,in vec2 coord) {
  //return texelFetch(tex,coord);
  return texelFetch(tex,ivec2(coord),0);
}

vec3 profileVariations(in sampler2D dirTex,in sampler2D validTex,in vec2 coord) {
  vec2 tangent = getTex(dirTex,gl_FragCoord.st).xy;

    const float eps = 0.0001;
    if(isnan(length(tangent)) || tangent==vec2(0.0) || length(tangent)<eps) {
      return vec3(0.0);
    }

  const int hlf = fitsize/2;

  vec2  s  = vec2(sw,sh);
  vec2  ss = vec2(1.0/sw,1.0/sh);
  vec4  t1 = vec4(tangent,getTex(dirTex,coord).zw);
  vec4  t2 = vec4(-t1.xy,t1.zw);
  vec2  c1 = coord+t1.xy;
  vec2  c2 = coord+t2.xy;

  vec3  sc = vec3(0.0);        // fit squeleton curvature
  vec3  ph = H[hlf].xyz*t1.z; // fit profile height variation
  vec3  pc = H[hlf].xyz*t1.w; // fit profile curvature variation

  vec3  h1,h2;
  vec4  tmp1,tmp2;
  vec2  c1s,c2s;
  vec2  coords = coord;//*ss;
  float d1,d2;
  vec2  vv1,vv2;
  vec2  pv1,pv2;

  const int m = 10;

  for(int i=1;i<=hlf;++i) {
    for(int j=0;j<m;++j) {
      //c1s = c1*ss;
      //c2s = c2*ss;

      vv1 = coords+dot(tangent,c1s-coords)*tangent;
      vv2 = coords+dot(tangent,c2s-coords)*tangent;

      pv1 = vv1-c1s;
      pv2 = vv2-c2s;

      // distances to tangents (in pixels)
      d1 = length(pv1);
      d2 = length(pv2);

      // new directions
      tmp1 = getTex(dirTex,c1);
      tmp2 = getTex(dirTex,c2);

      // checking null directions

      //if(j==m-1) {
      //tmp1 = (isnan(length(tmp1.xy)) || tmp1.xy==vec2(0.0) || length(tmp1.xy)<eps) ? vec4(t1.xy,tmp1.zw) : vec4(normalize(tmp1.xy)*s,tmp1.zw);
      //tmp2 = (isnan(length(tmp2.xy)) || tmp2.xy==vec2(0.0) || length(tmp2.xy)<eps) ? vec4(t2.xy,tmp2.zw) : vec4(normalize(tmp2.xy)*s,tmp2.zw);

      tmp1 = tmp1.xy==vec2(0.0) ? vec4(t1.xy,tmp1.zw) : vec4(normalize(tmp1.xy),tmp1.zw);
      tmp2 = tmp2.xy==vec2(0.0) ? vec4(t2.xy,tmp2.zw) : vec4(normalize(tmp2.xy),tmp2.zw);

      //} else {
      //tmp1 = vec4(t1.xy,tmp1.zw);
      //tmp2 = vec4(t2.xy,tmp2.zw);
      // }

      /*     if(testDir(tmp1.xy) || testDir(tmp2.xy)) */
      /*       return vec3(1.0); */

      /*     if(tmp1.xy==vec2(0.0)) { */
      /*       tmp1 = vec4(t1.xy,tmp1.zw); */
      /*     } else { */
      /*       tmp1 = vec4(normalize(tmp1.xy)*s,tmp1.zw); */
      /*     } */

      /*     if(tmp2.xy==vec2(0.0)) { */
      /*       tmp2 = vec4(t2.xy,tmp2.zw); */
      /*     } else { */
      /*       tmp2 = vec4(normalize(tmp2.xy)*s,tmp2.zw); */
      /*     } */

      // checking inverted directions
      t1.xy = dot(t1.xy,tmp1.xy)>=0.0 ? tmp1.xy : -tmp1.xy;
      t2.xy = dot(t2.xy,tmp2.xy)>=0.0 ? tmp2.xy : -tmp2.xy;

      //t1.xy = tmp1.xy;//dot(normalize(t1.xy),tmp1.xy)>=-0.5 ? tmp1.xy : -tmp1.xy;
      //t2.xy = tmp1.xy;//dot(normalize(t2.xy),tmp2.xy)>=-0.5 ? tmp2.xy : -tmp2.xy;

      // computing positions
      c1 += t1.xy;
      c2 += t2.xy;

    }

    // fitting
    h1  = H[hlf-i].xyz;
    h2  = H[hlf+i].xyz;
    sc += h1*d1;
    sc += h2*d2;
    ph += h1*t1.z;
    ph += h2*t2.z;
    pc += h1*t1.w;
    pc += h2*t2.w;
  }

  return vec3(tanh(abs(sc.x)),abs(ph.x),abs(pc.x));
}

vec3 testCurvature(in sampler2D dirTex,in sampler2D validTex,in vec2 coord) {

  vec2 tangent = getTex(dirTex,coord).xy;

  if(getTex(validTex,gl_FragCoord.st).w==0.0)
    return vec3(1.0);

  vec2 tan1 = tangent;
  vec2 tan2 = tangent;

  vec4 tmp1,tmp2;

  const int hlf = fitsize/2;

  vec2  s  = vec2(sw,sh);
  vec4  t1 = vec4(tangent,getTex(dirTex,coord).zw);
  vec4  t2 = vec4(-t1.xy,t1.zw);
  vec2  c1 = coord+t1.xy;
  vec2  c2 = coord+t2.xy;

  float total = 0.0;

  const int m = 5;

  for(int i=1;i<=hlf;++i) {
    for(int j=0;j<m;++j) {
      tmp1 = getTex(dirTex,c1);
      tmp2 = getTex(dirTex,c2);

/*       if(tmp1.xy==vec2(0.0)) */
/* 	tmp1.xy = normalize(t1.xy); */

/*       if(tmp2.xy==vec2(0.0)) */
/* 	tmp2.xy = normalize(t2.xy); */

      total += smoothstep(0.9,1.0,abs(dot(tan1,tmp1.xy)));
      total += smoothstep(0.9,1.0,abs(dot(tan2,tmp2.xy)));

      tan1 = tmp1.xy;
      tan2 = tmp2.xy;

      tmp1 = vec4(tmp1.xy,tmp1.zw);
      tmp2 = vec4(tmp2.xy,tmp2.zw);

      t1.xy = dot(t1.xy,tmp1.xy)>=0.0 ? tmp1.xy : -tmp1.xy;
      t2.xy = dot(t2.xy,tmp2.xy)>=0.0 ? tmp2.xy : -tmp2.xy;

      c1 += t1.xy;
      c2 += t2.xy;

    }
  }

  total = total/float(2*hlf*m);
  total = total;

  return vec3(total);


}

vec3 testValid(in sampler2D dirTex,in sampler2D validTex,in vec2 coord) {

  vec2 tangent = getTex(dirTex,coord).xy;
  //vec2 tangent = getTex(dirTex,gl_FragCoord.st).xy;

  /*   if(testDir(tangent)) */
  /*     return vec3(1.0); */

  //  return vec3(0.0,1.0,0.0);


/*   if(getTex(validTex,gl_FragCoord.st).w==0.0) */
/*     return vec3(1.0); */

  const int hlf = fitsize/2;

  vec2  s  = vec2(sw,sh);
  vec2  ss = vec2(1.0/sw,1.0/sh);
  //vec4  t1 = vec4(normalize(tangent)*s,texelFetch(tex,coord).zw);
  vec4  t1 = vec4(tangent,getTex(dirTex,coord).zw);
  vec4  t2 = vec4(-t1.xy,t1.zw);
  vec2  c1 = coord+t1.xy;
  vec2  c2 = coord+t2.xy;

  /*   vec3  sc = vec3(0.0);        // fit squeleton curvature */
  /*   vec3  ph = H[hlf].xyz*t1.z; // fit profile height variation */
  /*   vec3  pc = H[hlf].xyz*t1.w; // fit profile curvature variation */

  vec3  h1,h2;
  vec4  tmp1,tmp2;
  vec2  c1s,c2s;
  vec2  coords = coord;//*ss;
  float d1,d2;
  vec2  vv1,vv2;
  vec2  pv1,pv2;
  float ep1 = 1.0;
  float ep2 = 1.0;
  float ep = 0.0;

  const int m = 5;

  for(int i=1;i<=hlf;++i) {
    for(int j=0;j<m;++j) {
      ep1 = getTex(validTex,c1).w;
      ep2 = getTex(validTex,c2).w;
      //ep1 = min(ep1,getTex(validTex,c1).w);
      //ep2 = min(ep2,getTex(validTex,c2).w);
      ep += 1.0-ep1;
      ep += 1.0-ep2;

      // new directions
      tmp1 = getTex(dirTex,c1);
      tmp2 = getTex(dirTex,c2);

      tmp1 = vec4(tmp1.xy,tmp1.zw);
      tmp2 = vec4(tmp2.xy,tmp2.zw);

      if(ep1==0.0 || ep2==0.0)
        return vec3(0.0,0.0,0.0);

      // checking inverted directions
      t1.xy = dot(t1.xy,tmp1.xy)>=0.0 ? tmp1.xy : -tmp1.xy;
      t2.xy = dot(t2.xy,tmp2.xy)>=0.0 ? tmp2.xy : -tmp2.xy;

      //t1.xy = tmp1.xy;
      //t2.xy = tmp2.xy;

      // computing positions
      c1 += t1.xy;
      c2 += t2.xy;

    }
  }

  return vec3(1.0-ep/float(hlf*m));//*(1.0-getTex(validTex,gl_FragCoord.st).w);
  //return vec3(1.0-ep/float(hlf*m))*

  return vec3(1.0-ep/float(hlf*m))*vec3(abs(texelFetch(analysis2,ivec2(gl_FragCoord.st),0).z)*texelFetch(validTex,ivec2(gl_FragCoord.st),0).w);

}

vec3 centerAndRadius(in float uQuad,in vec2 uLinear,in float uConstant) {
  const float eps = 1e-5;
  vec2  c = vec2(0.0);
  float r = 0.0;

  if(abs(uQuad)>eps) {
    float b = 1.0/uQuad;
    c = -0.5*b*uLinear;
    r = sqrt(dot(c,c)-b*uConstant);

  }

  return vec3(c,r);
}

bool testNanInf(in float v) {
  return isnan(v) || isinf(v);
}

bool testNanInf(in vec2 v) {
  return testNanInf(v.x) || testNanInf(v.y) || testNanInf(length(v));
}

vec3 fitCircle2(in sampler2D dirTex,in sampler2D coordTex,in sampler2D validTex,in vec2 startCoord) {
  const float eps = 1e-5;
  const int nofSamples = 20;

  vec2  ps  = vec2(sw,sh);
  vec2  pss = vec2(1.0/sw,1.0/sh);

  vec2  tt;                        // temp tangent
  int   ns = 1;                    // nb samples

  vec2  c1 = startCoord;           // position in texture space
  vec2  c2 = c1;
  vec2 c1tmp;
  vec2 c2tmp;
  vec2  p = c1;//*pss;                // position in image space
  //vec2  p2 = p1;

  vec2  t1 = getTex(dirTex,c1).xy;  // tangent
  vec2  t2 = t1;

  vec2  n = vec2(-t1.y,t1.x);       // normal
  //vec2  n2 = n1;

  float w1 = getTex(validTex,c1).w; // weight
  float w2 = w1;
  float w  = w1;

  float test1 = -1;
  float test2 = -1;
  bool  cd       = true;
  bool  lop      = true;
  bool  rop      = true;

  if(getTex(validTex,gl_FragCoord.st).w<eps) {
    lop = rop = false;
    test1 = test2 = 0.0;
  }

  if(w<eps) {
    lop = rop = false;
    test1 = test2 = 0.0;
    return vec3(1.0,0.0,1.0);
  }

  vec2  sumP     = w*p;
  vec2  sumN     = w*n;
  float sumDotPN = w*dot(p,n);
  float sumDotPP = w*dot(p,p);
  float sumW     = w;

  for(int i=1;i<=nofSamples;++i) {
    if(!lop && !rop) {
      break;
    }

    if(cd) {
      c1tmp = getTex(coordTex,c1+t1).xy;
      w1 = min(w1,getTex(validTex,c1tmp).w);

      if(w1<eps || c1tmp==c1) {
	cd  = !cd;
	lop = false;
	test1 = float(i)/float(nofSamples);
	continue;
      }

      c1 = c1tmp;
      tt = getTex(dirTex,c1).xy;

      t1 = dot(t1,tt)<0.0 ? normalize(-tt) : normalize(tt);
/*       tt = dot(t1,tt)<0.0 ? -tt : tt; */
/*       t1 = normalize(mix(t1,tt,w1)); */
      p  = c1;//*pss;
      n  = vec2(-t1.y,t1.x);
      w  = w1;

      if(rop) {
	cd = !cd;
      }

    } else {
      c2tmp = getTex(coordTex,c2-t2).xy;
      w2 = min(w2,getTex(validTex,c2tmp).w);

      if(w2<eps || c2==c2tmp) {
	cd  = !cd;
	rop = false;
	test2 = float(i)/float(nofSamples);
	continue;
      }

      c2 = c2tmp;
      tt = getTex(dirTex,c2).xy;

      t2 = dot(t2,tt)<0.0 ? normalize(-tt) : normalize(tt);
/*       tt = dot(t2,tt)<0.0 ? -tt : tt; */
/*       t2 = normalize(mix(t2,tt,w2)); */
      p  = c2;//*pss;
      n  = vec2(-t2.y,t2.x);
      w  = w2;

      if(lop) {
	cd = !cd;
      }
    }

    sumP     += w*p;
    sumN     += w*n;
    sumDotPN += w*dot(n,p);
    sumDotPP += w*dot(p,p);
    sumW     += w;
    ns       ++;

  }

  float invSumW   = sumW<0.1 ? 1.0/0.1 : 1.0/sumW;
  float uQuad     = 0.5 * (sumDotPN - invSumW*dot(sumP,sumN))/(sumDotPP - invSumW*dot(sumP,sumP));
  vec2  uLinear   = invSumW*(sumN-2.0*uQuad*sumP);
  float uConstant = -invSumW*(dot(uLinear,sumP)+uQuad*sumDotPP);

  vec3 cr = centerAndRadius(uQuad,uLinear,uConstant);

  //if(abs(uQuad)<0.00001)
  //return vec3(0,1,0);

  //return vec3(abs(uLinear.x),abs(uLinear.y),0.0);


/*   if(cr.z==0.0) */
/*     return vec3(0.0,1.0,0.0); */

  //return vec3(1.0/cr.z);
  //return vec3(getTex(validTex,startCoord).w);

  //return vec3(max(test1,test2));



/*   if(test1!=-1 && test2!=-1) */
/*     return vec3(1.0,0.0,0.0); */

/*   if(test1!=-1) */
/*     return vec3(0.0,1.0,0.0); */

/*   if(test2!=-1) */
/*     return vec3(0.0,1.0,0.0); */


/*   return vec3(1.0); */
  return cr.z==0.0 ? vec3(0.0) : vec3(1.0/cr.z)*vec3(getTex(validTex,startCoord).w);
}

vec3 fitCircle3(in sampler2D dirTex,in sampler2D coordTex,in sampler2D validTex,in vec2 startCoord) {

  const float eps = 1e-5;
  const int nofSamples = 50;

  vec2  ps  = vec2(sw,sh);
  vec2  pss = vec2(1.0/sw,1.0/sh);

  vec2  tt;                        // temp tangent
  int   ns = 1;                    // nb samples

  vec2  c1 = startCoord;           // position in texture space
  vec2  c2 = c1;
  vec2 c1tmp;
  vec2 c2tmp;
  vec2  p = c1;//*pss;                // position in image space
  //vec2  p2 = p1;

  vec2  t1 = normalize(getTex(dirTex,c1).xy);  // tangent
  vec2  t2 = t1;

  vec2  n = vec2(-t1.y,t1.x);       // normal
  //vec2  n2 = n1;

  float w1 = getTex(validTex,c1).w; // weight
  float w2 = w1;
  float w  = w1;

  float test1 = -1;
  float test2 = -1;
  bool  cd       = true;
  bool  lop      = true;
  bool  rop      = true;

  if(length(t1)<eps || w<eps) {
    lop = rop = false;
    test1 = test2 = 0.0;
    return vec3(0.0,0.0,0.0);
  }

  vec2  sumP     = w*p;
  vec2  sumN     = w*n;
  float sumDotPN = w*dot(p,n);
  float sumDotPP = w*dot(p,p);
  float sumW     = w;

  for(int i=1;i<=nofSamples;++i) {
    if(w<eps) break;

    if(!lop && !rop) {
      break;
    }

    if(cd) {
      c1tmp = c1+t1;//*ps;
      w1 = min(w1,getTex(validTex,c1tmp).w);

      c1 = c1tmp;
      tt = getTex(dirTex,c1).xy;
      if(w1<eps || length(tt)<0.9)
	tt = t1;
      tt = dot(t1,tt)<0.0 ? -tt : tt;
      t1 = normalize(tt);
      p  = c1;//*pss;
      n  = vec2(-t1.y,t1.x);
      w  = w1;

      if(rop) {
	cd = !cd;
      }

    } else {
      c2tmp = c2-t2;//*ps;
      w2 = min(w2,getTex(validTex,c2tmp).w);


      c2 = c2tmp;
      tt = getTex(dirTex,c2).xy;
      if(w2<eps || length(tt)<0.9)
	tt = t2;
      tt = dot(t2,tt)<0.0 ? -tt : tt;
      t2 = normalize(tt);
      p  = c2;//*pss;
      n  = vec2(-t2.y,t2.x);
      w  = w2;

      if(lop) {
	cd = !cd;
      }
    }

    sumP     += w*p;
    sumN     += w*n;
    sumDotPN += w*dot(n,p);
    sumDotPP += w*dot(p,p);
    sumW     += w;

    if(w>eps)
      ns ++;

  }

/*   if(w1<0.9 || w2<0.9) { */
/*     // end/T junction */
/*     return vec3(0.0,1.0,0.0); */
/*   } */
  //return vec3(ns/(1.0+float(nofSamples)));
  //return vec3(sumW/(1.0+float(nofSamples)));

  float invSumW   = 1.0/sumW;
  float uQuad     = 0.5 * (sumDotPN - invSumW*dot(sumP,sumN))/(sumDotPP - invSumW*dot(sumP,sumP));
  vec2  uLinear   = invSumW*(sumN-2.0*uQuad*sumP);
  float uConstant = -invSumW*(dot(uLinear,sumP)+uQuad*sumDotPP);

  //return vec3(uQuad);

  vec3 cr = centerAndRadius(uQuad,uLinear,uConstant);

  //if(abs(uQuad)<0.00001)
  //return vec3(0,1,0);

  //return vec3(abs(uLinear.x),abs(uLinear.y),0.0);


/*   if(cr.z==0.0) */
/*     return vec3(0.0,1.0,0.0); */

  //return vec3(1.0/cr.z);
  //return vec3(getTex(validTex,startCoord).w);

  //return vec3(max(test1,test2));



/*   if(test1!=-1 && test2!=-1) */
/*     return vec3(1.0,0.0,0.0); */

/*   if(test1!=-1) */
/*     return vec3(0.0,1.0,0.0); */

/*   if(test2!=-1) */
/*     return vec3(0.0,1.0,0.0); */

  // return vec3(cr.z*0.001);
/*   return vec3(1.0); */
  //cr.z = tanh(cr.z*0.01);

  return cr.z==0.0 ? vec3(0.0) : vec3(1.0/cr.z);//*vec3(getTex(validTex,startCoord).w);
}

vec3 fitCircles(in sampler2D dirTex,in sampler2D coordTex,in sampler2D validTex,in vec2 startCoord) {
  const float eps = 1e-5;

  float w = getTex(validTex,gl_FragCoord.st).w;

  //vec2 pos1 = getTex(coordTex,gl_FragCoord.st).xy;
  vec2 pos1 = gl_FragCoord.st;

  vec2 pos2 = pos1 + 1.0*(getTex(coordTex,getTex(coordTex,pos1).xy).xy-pos1)+vec2(1,1);
  vec2 pos3 = pos1 + 1.0*(getTex(coordTex,getTex(coordTex,pos1).xy).xy-pos1)-vec2(1,1);

  vec3 c1 = fitCircle3(dirTex,coordTex,validTex,pos1);
  vec3 c2 = fitCircle3(dirTex,coordTex,validTex,pos2);
  vec3 c3 = fitCircle3(dirTex,coordTex,validTex,pos3);


/*   vec2 tmp = getTex(coordTex,getTex(coordTex,pos1).xy).xy-pos1; */

/*   vec2 f1 = pos1+tmp+normalize(tmp)*vec2(sw,sh)*1.1; */
/*   vec2 f2 = pos1+tmp-normalize(tmp)*vec2(sw,sh)*1.1; */

/*   vec3 c1 = fitCircle3(dirTex,coordTex,validTex,f1); */
/*   vec3 c2 = fitCircle3(dirTex,coordTex,validTex,f2); */

/*   float moy = 0.5*(c1.x+c2.x); */

/*   return vec3(tanh(moy*20))*vec3(getTex(validTex,startCoord).w); */

/*   float d = abs(c1.x-moy) + abs(c2.x-moy); */
/*   return vec3(d*10)*vec3(getTex(validTex,startCoord).w); */




/*   //if(abs(c1.x)-abs(c2.x)) */

  float m = 0.3333*(c1.x+c2.x+c3.x);
  float d = abs(c1.x-m) + abs(c2.x-m) + abs(c3.x-m);

  float test = tanh(abs(0.3333*(c1+c2+c3)).x*50.0);

  //return vec3(d*10);//*vec3(getTex(validTex,startCoord).w);
  //return c2*vec3(getTex(validTex,startCoord).w);

  return vec3(getTex(validTex,startCoord).w);
  return vec3(test);
  return abs(0.3333*(c1+c2+c3))*vec3(getTex(validTex,startCoord).w);
}

vec3 fitCircle(in sampler2D dirTex,in sampler2D coordTex,in sampler2D validTex,in vec2 startCoord) {
  const float eps = 1e-5;
  const int nofSamples = 10;

  vec2  ps  = vec2(sw,sh);
  vec2  pss = vec2(1.0/sw,1.0/sh);

  vec2 ctmp;
  vec2  tt;                        // temp tangent
  vec2  c  = startCoord;           // position in texture space
  vec2  p  = c*pss;                // position in image space
  vec2  t  = getTex(dirTex,c).xy;  // tangent
  vec2  n  = vec2(-t.y,t.x);       // normal
  int   ns = 1;                    // nb samples
  float w  = getTex(validTex,c).w; // weight

  vec2  sumP     = w*p;
  vec2  sumN     = w*n;
  float sumDotPN = w*dot(p,n);
  float sumDotPP = w*dot(p,p);
  float sumW     = w;

  for(int i=1;i<=nofSamples;++i) {
    ctmp = getTex(coordTex,c+t*ps).xy;

    w = min(w,getTex(validTex,ctmp).w);
    tt = getTex(dirTex,ctmp).xy;
    if(tt==vec2(0.0) || distance(c,ctmp)<0.5*length(ps))
      tt = t;

    c  = ctmp;
    p  = c*pss;
    t  = dot(t,tt)<0.0 ? -tt : tt;
    n  = vec2(-t.y,t.x);

    sumP     += w*p;
    sumN     += w*n;
    sumDotPN += w*dot(n,p);
    sumDotPP += w*dot(p,p);
    sumW     += w;
    ns       ++;
  }

  c = startCoord;
  t = getTex(dirTex,c).xy;
  w = getTex(validTex,c).w;

  for(int i=1;i<=nofSamples;++i) {
    ctmp  = getTex(coordTex,c-t*ps).xy;
    w = min(w,getTex(validTex,ctmp).w);
    tt = getTex(dirTex,ctmp).xy;
    if(tt==vec2(0.0) || distance(c,ctmp)<0.5*length(ps))
      tt = t;
    p  = c*pss;
    t  = dot(t,tt)<0.0 ? -tt : tt;
    n  = vec2(-t.y,t.x);

    sumP     += w*p;
    sumN     += w*n;
    sumDotPN += w*dot(n,p);
    sumDotPP += w*dot(p,p);
    sumW     += w;
    ns       ++;
  }

  float invSumW   = sumW<0.1 ? 1.0/0.1 : 1.0/sumW;
  float uQuad     = 0.5 * (sumDotPN - invSumW*dot(sumP,sumN))/(sumDotPP - invSumW*dot(sumP,sumP));
  vec2  uLinear   = invSumW*(sumN-2.0*uQuad*sumP);
  float uConstant = -invSumW*(dot(uLinear,sumP)+uQuad*sumDotPP);

  vec3 cr = centerAndRadius(uQuad,uLinear,uConstant);

  return cr.z==0.0 ? vec3(0.0) : vec3(1.0/cr.z)*vec3(getTex(validTex,startCoord).w);
}



/* float filterWeakLines(in sampler2D tex,in vec2 coord,in vec2 tangent) { */
/*   // keep lines which have the maximum intensity  */
/*   const int size = 10; */
/*   const float eps = 0.1; */

/*   float f  = abs(texelFetch(tex,coord).z); */
/*   float p1 = f; */
/*   float p2 = f; */
/*   vec2  s  = vec2(sw,sh); */
/*   vec2  t1 = tangent*s; */
/*   vec2  t2 = -t1; */
/*   vec2  c1 = coord+t1; */
/*   vec2  c2 = coord+t2; */
/*   vec3  d1,d2; */
/*   vec2 tmp1,tmp2; */

/*   // convolve intensity along the line */
/*   for(int i=0;i<size;++i) { */
/*     d1  = texelFetch(tex,c1).xyz; */
/*     d2  = texelFetch(tex,c2).xyz; */

/*     tmp1 = (isnan(length(d1.xy)) || d1.xy==vec2(0.0) || length(d1.xy)<eps) ? t1 : d1.xy*s; */
/*     tmp2 = (isnan(length(d2.xy)) || d2.xy==vec2(0.0) || length(d2.xy)<eps) ? t1 : d2.xy*s; */

/*     t1  = dot(t1,tmp1)>=0.0 ? tmp1 : -tmp1; */
/*     t2  = dot(t2,tmp2)>=0.0 ? tmp2 : -tmp2; */

/*     c1 += t1; */
/*     c2 += t2; */

/*     f  += (abs(d1.z)+abs(d2.z)); */
/*   } */

/*   f = f/(2.0*float(size)+1.0); */
/*   f = smoothstep(max(ff-fs,0.0),min(ff+fs,1.0),f); */

/*   return f; */
/* } */


// uniform float silFilter; //## min : 0 ## max : 1.0 ## default : 0.01
// uniform float lineFilter; //## min : 0 ## max : 1.0 ## default : 0.01

// uniform float inputH; //## min : 0 ## max : 1.0 ## default : 0.0
// uniform float outputH; //## min : 0 ## max : 1.0 ## default : 0.0
// uniform float inputC; //## min : 0 ## max : 1.0 ## default : 0.0
// uniform float outputC; //## min : 0 ## max : 1.0 ## default : 0.0
// uniform float dmax; //## min : 1.0 ## max : 5.0 ## default : 1.57

 float Smin=0.8; //## min : 0 ## max : 1.0 ## default : 0.8
 float Smax=1.0; //## min : 0 ## max : 1.0 ## default : 1.0

 float Hmin=0; //## min : 0 ## max : 1.0 ## default : 0.0
 float Hmax=1.0; //## min : 0 ## max : 1.0 ## default : 0.0

 float Cmin=0; //## min : 0 ## max : 1.0 ## default : 0.0
 float Cmax=1.0; //## min : 0 ## max : 1.0 ## default : 0.0

 float Dmin=0; //## min : 0 ## max : 5.0 ## default : 0.0
 float Dmax=2.0; //## min : 0 ## max : 5.0 ## default : 2.0


/* float scenar1(in float dist, in float height, in float curvature, in float valid) { */
/*   //const float dmax = 1.2; */

/*   float dh; */
/*   float dx; */

/* /\*   if(valid<0.0) { *\/ */
/* /\*     dh = smoothstep(silFilter-0.1,silFilter+0.1,height); *\/ */
/* /\*   } else { *\/ */
/* /\*     //dh = smoothstep(lineFilter-0.2,lineFilter+0.2,height); *\/ */
/* /\*     dh = smoothstep(input,output,height); *\/ */
/* /\*   } *\/ */

/* /\*   dx = 1.0-smoothstep(dmax-0.3,dmax,dist); *\/ */

/*   dh = step(outputH,abs(valid)*height); */
/*   dx = 1.0-step(dmax,dist); */

/*   return dh*dx; */
/* } */

/* float scenar2(in float dist, in float height, in float curvature, in float valid) { */
/*   float dh; */
/*   float dx; */

/*   dh = smoothstep(inputH,outputH,abs(valid)*height); */
/*   dx = 1.0-step(dmax,dist); */

/*   return dh*dx; */
/* } */

/* float scenar3(in float dist, in float height, in float curvature, in float valid) { */
/*   float dh; */
/*   float dx; */
/*   float dc; */

/*   dh = smoothstep(inputH,outputH,abs(valid)*height); */
/*   dx = 1.0-step(dmax,dist); */

/*   if(valid<0.0) */
/*     dc = dmax*(1.0-smoothstep(inputC,outputC,abs(valid)*abs(curvature))); */
/*   else  */
/*     dc = dmax*(1.0-smoothstep(inputC,outputC,abs(valid)*abs(curvature))); */

/*   return dh*exp(-(dist*dist)/(2.0*dc*dc)); */
/* } */

/* float scenar4(in float dist, in float height, in float curvature, in float valid) { */
/*   float dh = smoothstep(inputH,outputH,abs(height)); */
/*   float dc = smoothstep(inputC,outputC,abs(curvature)); */

/*   const float PI = 3.14159265; */
/*   float sigmaMin = 1.0; */
/*   float tkx = sigmaMin+(1.0-dh*dc)*dmax; */
/*   float kx2 = tkx*tkx; */

/*   return 1.0/sqrt(PI*kx2)*exp(-dist*dist/(kx2)); */
/* } */

float weightFunction(in float dist, in float height, in float curvature, in float valid) {
  float smin = Smin;
  float hmin = Hmin;
  float cmin = Cmin;
  float dmin = Dmin;

  float smax = max(smin,Smax);
  float hmax = max(hmin,Hmax);
  float cmax = max(cmin,Cmax);
  float dmax = max(dmin,Dmax);

  float fh;

  if(valid>=0.0) {
    fh = smoothstep(hmin,hmax,tanh(abs(height)*abs(valid)));
  } else {
    fh = smoothstep(smin,smax,tanh(abs(height)*abs(valid)));
  }

  float fc = smoothstep(cmin,cmax,tanh(abs(curvature))*10.0);
  float fd = 1.0-smoothstep(dmin,dmax,abs(dist));

  return fh*fc*fd;
}

void main(void) {
  vec4 a1 = texelFetch(analysis1,ivec2(gl_FragCoord.st),0);
  vec4 a2 = texelFetch(analysis2,ivec2(gl_FragCoord.st),0);

  // validity
  float validity = a1.w;

  // weight function
  float pdist   = a1.z;
  float pheight = abs(a2.z);
  float pcurv   = a2.w;
  float weight  = weightFunction(pdist,pheight,pcurv,validity);

  vec4 color = texelFetch(shading,ivec2(gl_FragCoord.st),0);
/*   float h = texelFetch(desc1,gl_FragCoord.st).w; */
/*   vec4 color = vec4(tanh((tanh(h*1.0)*0.5+0.5)*1.9)); */

  if(texelFetch(shading,ivec2(gl_FragCoord.st),0).w==0.0)
    fragColor = vec4(weight,1.0,1.0,1.0);
  else
    fragColor = vec4(weight,color.x,color.y,color.z);

  fragColor = vec4(weight,color.x,color.y,color.z);



  //vec4 a3 = texelFetch(analysis3,gl_FragCoord.st);

/*   // computing curvature  */
/*   vec3 v = f*profileVariations(analysis2,analysis1,a1.xy); */

/*   // depth  */
/*   float d = texelFetch(grad,a1.xy).w; */

/*   // shading  */
/*   float s = texelFetch(shading,gl_FragCoord.st).x; */

/*   //vec3 test = (testValid(analysis2,analysis1,a1.xy)); */
/*   //vec3 test = (testCurvature(analysis2,analysis1,a1.xy)); */
/*   //vec3 test = (testValid(analysis2,analysis1,gl_FragCoord.st)); */
/*   vec3 test = fitCircles(analysis2,analysis1,analysis1,gl_FragCoord.st);//a1.xy); */
/*   //vec3 test = (fitCircle2(analysis2,analysis1,analysis1,a1.xy)); */
/*   //fragColor = vec4(test,f); */

/*   fragColor = vec4(f); */

/*   /\*   if(f==0.0) *\/ */
/*   /\*     fragColor = vec4(1.0,0.0,0.0,1.0); *\/ */
/*   //fragColor = vec4(f); */
/*   //fragColor = vec4(f,v.x,d,s); */
/*   //fragColor = vec4(f,v.x,test.x,s); */
}
