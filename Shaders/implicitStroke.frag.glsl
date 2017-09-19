uniform sampler2D texBrush;

layout (location = 0) in vec4 in_texCoord;

in vec3 color;
in float b;
in float type;
////these should be inputs
float tone = 1; //## min : 0 ## max : 1 ## default :1
float hardness = 0.06; //## min : 0 ## max : 1 ## default :0.06
int brushType;
//varying vec2 bordel;

layout (location = 0) out vec4 fragColor;

float gauss(vec2 v){
    float r = v.x*v.x+v.y*v.y;
    return max(exp(-r/(.050)),0.0);
}

float fGauss(vec2 v){
    v = 2.0*(v-vec2(.5));
    float minGauss = gauss(vec2(-1.0, 0.0));
    float maxGauss = gauss(vec2(0.0));
    return (100.0*hardness+1.0)*((gauss(v)-minGauss)/(maxGauss-minGauss));
}

float fTex(vec2 v){
    return 1.0-texelFetch(texBrush, ivec2(v),0).r;
}

// use for symmetrical
float fTexLocal(vec2 v){
    return 1.0-(texelFetch(texBrush, ivec2(v),0).r+texelFetch(texBrush, ivec2(1.0-v), 0).r)/2.0;
}

const int colorType = 0;
vec3 colorFunction(vec2 v){

    if(colorType == 0)  //black
	return vec3(0.0);
    if(colorType == 1) // from vert (from feature pos)
	return color;
	return vec3(0.0);
//    if(colorType == 2) // from footprint
//	return texture2D(texBrush, v).rgb;
}

float getBrush(in vec2 v) {
  if(brushType==0) return fTex(v);
  if(brushType==1) return fTexLocal(v);
  if(brushType==2) return fGauss(v);
  return 0.0;
}

void main(void)
{

    float alpha;
    fragColor = vec4(vec3(0.0),5.0/255.0);
//    if(type == 8.0){ // silhouette

    alpha = tone*b*1;//getBrush(in_texCoord.st);

    fragColor = vec4(alpha*colorFunction(in_texCoord.st),alpha);
}
