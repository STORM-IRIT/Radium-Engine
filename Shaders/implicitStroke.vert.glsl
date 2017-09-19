#include "Structs.glsl"
uniform sampler2D texData1;
uniform sampler2D texData2;
uniform sampler2D texData3;
uniform sampler2D texData4;
uniform sampler2D texData5;
uniform sampler2D texNoise;

uniform vec2 wh;

uniform Transform transform;

layout (location = 0) in vec4 in_position;
layout (location = 5) in vec4 in_texCoord;

layout (location = 0) out vec4 out_texCoord;

uniform int wfType;
uniform int rotateType;
int radiusType=0;

float radius=5; //## min : 0.0 ## max : 100 # default :10
// uniform float radiusSil; //## min : 0.5 ## max : 100 ## default :48.09
uniform float wobbling; //## min : 0 ## max : 30 ## default :0
uniform float sigmaMax; //## min : 0 ## max : 30 ## default :0
uniform float tanhmax; //## min : 0 ## max : 2.5 ## default :0.15
uniform float curvF; //## min : -1 ## max : 1 ## default :1
// uniform float harderF; //## min : 0 ## max : 1 ## default :0.16
// uniform float curvF1; //## min : 0 ## max : 100 ## default :99.97
// uniform float curvF2; //## min : 0 ## max : 10 ## default :10
uniform float stipple; //## min : 0 ## max : 1 ## default :1
uniform float angle; //## min : 0 ## max : 7 ## default :3.27

out vec3 color;
out float b;
out float type;

/* vec3 rgbToHsv(in float r,in float g,in float b) { */
/*   float h,s,v; */

/*   v = max(max(r,g),b); */
/*   float delta = v-min(min(r,g),b); */
/*   float eps = 0.000001; */

/*   if(delta<eps) { */
/*     s = 0.0; */
/*     h = 0.0; */
/*   } else { */
/*     s = delta/v; */

/*     if(r==v)      h = (g-b)/delta; */
/*     else if(g==v) h = 2.0 + (b-r)/delta; */
/*     else          h = 4.0 + (r-g)/delta; */

/*     h *= 60.0; */
/*     if(h<0.0) */
/*       h += 360; */
/*   } */

/*   return vec3(h,s,v); */
/* } */

/* vec3 hsvToRgb(in float h,in float s,in float v) { */
/*   vec3 rgb; */
/*   int hi = int(floor(h/60.0))%6; */
/*   float f = h/60.0 - floor(h/60.0); */
/*   float p = v*(1.0-s); */
/*   float q = v*(1.0-f*s); */
/*   float t = v*(1.0-(1.0-f)*s); */

/*   if(hi==0) rgb = vec3(v,t,p); */
/*   else if(hi==1) rgb = vec3(q,v,p); */
/*   else if(hi==2) rgb = vec3(p,v,t); */
/*   else if(hi==3) rgb = vec3(p,q,v); */
/*   else if(hi==4) rgb = vec3(t,p,v); */
/*   else rgb = vec3(v,p,q); */

/*   return rgb; */
/* } */


float localGr;

struct lineData{
    float d;
    float curv;
    float modelCurv;
    float valid;
    int type;
    vec2 tangente;
    vec2 dirMax;
    vec2 n;
    vec2 dir;
    vec2 pos;
    vec2 fittedPos;
    vec4 noises;
  float depth;
};

lineData readLineData(vec2 pixelTexCoord, vec4 data1, vec4 data2, vec4 data3, vec4 data4){
    lineData ret;

    vec2 fittedPointCoord = data1.xy * wh;
    ret.fittedPos = fittedPointCoord;
    ret.d         = abs(data1.z);
    ret.modelCurv = data2.z;
    ret.tangente  = (data2.xy);
    ret.dirMax    = (data3.xy);
    ret.pos       = pixelTexCoord*wh;
    ret.dir       = normalize(fittedPointCoord - ret.pos);
    ret.valid     = data4.x;
    //ret.curv      = data4.y;
    ret.curv      = data2.w;
    ret.depth = data4.z;

    ret.type = int(data1.w);
    color = vec3(data4.yzw);

    ret.noises = texture(texNoise, (fittedPointCoord+ret.dirMax*2.0)/wh);

/*     color = rgbToHsv(color.x,color.y,color.z); */
/*     color.y *= ret.noises.x; */
/*     color = hsvToRgb(color.x,color.y,color.z); */

// simple screen checkboard
//  const float stippleSize = 40.0;
//  float sx = step(.5,fract(pixelTexCoord.x*wh.x/stippleSize));
//  float sy = step(.5,fract(pixelTexCoord.y*wh.y/stippleSize));
//  ret.noises.x = (sx+sy-2*sy*sx)-.5;
//    ret.noises = vec4(0.5);

    vec2 d   = ret.fittedPos-ret.pos;
    if(length(d)>0) d = normalize(d);
    vec2 tt  = vec2(-d.y,d.x);
    vec2 tangente  = dot(tt,ret.tangente)>=0.0 ? ret.tangente : -ret.tangente;
    ret.n =  vec2(tangente.y, -tangente.x);
    return ret;
}


float curvatureVariation(lineData data){
    return curvF*clamp(abs(data.curv*100),0,1);
//    return smoothstep(curvF1, curvF1+curvF2, abs(data.curv*50.0))*curvF*100;
}

float footprintNoiseFactor(lineData data){
    return clamp(1.0+stipple*data.noises.x*100.0, 0.0, 1.0);
}

lineData readLineData(vec2 pixelTexCoord){

//    vec2 tt = floor(pixelTexCoord*wh);
//    tt += .5;
//    tt /= wh;
//    pixelTexCoord = tt;

    vec4 data1 = texture(texData1, pixelTexCoord);
    vec4 data2 = texture(texData2, pixelTexCoord);
    vec4 data3 = texture(texData3, pixelTexCoord);
    vec4 data4 = texture(texData4, pixelTexCoord);
    return readLineData(pixelTexCoord, data1, data2, data3, data4);
}

vec2 rotate(vec2 v, float angle){
    mat2 rot = mat2 (cos(angle),  sin(angle),
		     -sin(angle), cos(angle));

    return rot*v;
}


vec2 rotateGlobal(vec2 v, lineData data){
    return rotate(v, angle);
}


float getAngleBetweenVec(vec2 l, vec2 ref){
    return atan(l.y, l.x) - atan(ref.y, ref.x);
}

vec2 rotateLocal(vec2 v, lineData data){
    float langle = getAngleBetweenVec(data.tangente, vec2(1,0));
    return rotate(v, (langle+angle));
}

float tanh(float c) {
  // remapping [-inf,inf] in [-1,1]

  float x = ((c*1.0)/tanhmax);
  float e = exp(-2.0*x);
  return clamp((1.0-e)/(1.0+e),-1.0,1.0);
}



float gaussRF(lineData data){
    float sigmaMin = 1;
    float tkx = sigmaMin+(1.0-abs(100.0*tanh(data.modelCurv)))*sigmaMax;
    float kx2 = tkx*tkx;

    return kx2;
}

/* float inverseWF(lineData data){ */
/*     return (tanh(abs(data.modelCurv)))/(1.0+abs(data.d)); */
/* } */

const float PI = 3.14159265;
/* float gaussWF(lineData data){ */
/*     float sigmaMin = 1; */
/*     float tkx = sigmaMin+(1.0-abs(tanh(data.modelCurv)))*sigmaMax; */
/*     float kx2 = tkx*tkx; */

/*     return 1.0/sqrt(PI*kx2)*exp(-data.d*data.d/(kx2)); */
/* } */

/* float diracWF(lineData data){ */
/*     float maxDist = 5.0; */
/*     return 1.0-smoothstep(0.0, maxDist, data.d); */
/* } */

/* float fillWF(lineData data){ */
/*     float tkx = 1.0-smoothstep(10.0, 10.0+50.0*sigmaMax, 500*abs(data.curv)); */
/*     return 1.-step(tkx, data.d); */
/* } */


/* //const int wfType = 0; */
/* float weightFunction(lineData data) */
/* {  */
/*     if(wfType == 0) */
/* 	return diracWF(data); */
/*     if(wfType == 1) */
/* 	return gaussWF(data); */
/*     if(wfType == 2) */
/* 	return fillWF(data); */
/*     if(wfType == 3) */
/* 	return inverseWF(data); */
/* } */


//const int rotateType =0;
vec2 rotate(vec2 v, lineData data){
    if(rotateType == 0) return rotateGlobal(v, data);
    if(rotateType == 1) return rotateLocal(v, data);
    return rotateGlobal(v, data);
}

//const int radiusType = 0;
float radiusFunction(lineData data){
    if(radiusType == 0) return radius;
    //if(radiusType == 1) return gaussRF(data);
    //if(radiusType == 1) return max(radius*(abs(data.valid)*abs(data.modelCurv)*30),1.0);
    if(radiusType == 1) return 2.0+max(radius*(abs(data.valid)*abs(data.modelCurv)*2),3.0);
    if(radiusType == 2) return radius*(smoothstep(0.5, 0.75, data.depth));
    return radius;
}

const int offsetFootprintType = 1;
vec2 offsetFootprint(lineData data){

    if(offsetFootprintType == 0) // typically with diracWF
	return data.fittedPos;
    if(offsetFootprintType == 1) // other case
	return data.pos;
    if(offsetFootprintType == 2) // should be better but shows fitting error
	return data.pos+(data.d-floor(data.d))*(data.dir);
    return data.fittedPos;
}

//uniform float appear; //## min : 0 ## max : 1 ## default :1

void main(void)
{

    vec2 pixelPos = in_texCoord.zw;
    lineData data = readLineData((pixelPos+.5)/wh);
    out_texCoord  = in_texCoord;
    b = data.valid;


/*     float testNoise = abs(data.noises.x); */
/*     if(appear<testNoise) { */
/*       gl_Position = vec4(-10); */
/*       return; */
    //}


    if((data.d<50.0)&& (data.valid>0.1)){
        // radius variation
        localGr = radiusFunction(data)/min(wh.x, wh.y);
        float localWobbling = wobbling;
        // here change for only a type of line, silhouette ...
        type = 0.0;
    //	if(data.type == 8){
    //	    type = data.type;
    //	    localGr = radiusSil;
            //localWobbling = 0.0;
    //	}
        vec2 v =  vec2(localGr,localGr)*(in_position.xy-vec2(.5));

        v = rotate(v, data);

    //    v += offsetFootprint(data);
/*
        float wobblingNorm = (data.noises.y*.5+.5);
        float wobblingAngle = ((data.noises.z)*.5+.5)*4.0*PI;
        vec2 wobblingDir = wobblingNorm*vec2(cos(wobblingAngle), sin(wobblingAngle));

        v += vec2(localWobbling * wobblingDir);
        b*= footprintNoiseFactor(data);
        //b*= weightFunction(data);
    */
    // for testing without wf
    //	b = 1;
        gl_Position = vec4((data.pos*2)/wh-vec2(1) + v, 0.0, 1.0);
    }
    else{
	    gl_Position = vec4(-10);
    }

}
