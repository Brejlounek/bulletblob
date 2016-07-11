#version 330

in vec2 UV;
in vec3 wPos0;
in vec3 wPos1;

layout(location = 0) out vec4 color;

uniform vec3 ambient;
uniform vec3 dirL;
uniform vec3 dirColor;
uniform mat4 lDirMVP;
uniform vec2 lDirPixel;
uniform vec2 textureSize;
uniform vec2 pixel;

uniform mat4 MVP;
uniform vec3 camPos;
uniform vec2 zClip;
uniform float zClipRatio;
uniform float iRand;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D aoTexture;
uniform sampler2DShadow lDirDepthTexture;

uniform sampler2D randomTexture;

//float linstep(float minn, float maxx, float v)
//{
//	float uh = clamp((v - minn)/(maxx - minn), 0, 1);
//	return uh;
//}
//
//float VSM(float myDepth, vec2 lUV)
//{
//	float k;
//	k=1;
//
//	vec3 moments = texture2D( lDirDepthTexture, lUV ).rgb;
//	if ( exp(k*(2*myDepth-1)) <= moments.x)
//		return 1;
//
//	float variance = moments.y - (moments.x*moments.x);
//	variance = max(variance,0.0000001);
//	variance = min(variance,1);
//
//	float d = moments.x - exp(k*(2*myDepth-1));
//	float p_max = variance / (variance + d*d);
//	p_max = linstep(0.0, 1, p_max);
//
//	return p_max;
//}

vec2 poisson8[] = vec2[]( vec2(0.7318033f, -0.3183815f),
vec2(0.888394f, 0.3714616f),
vec2(0.1823359f, -0.3939089f),
vec2(0.1777777f, 0.1619135f),
vec2(-0.4546505f, 0.4402646f),
vec2(0.2481735f, 0.9543526f),
vec2(-0.6516684f, -0.3220734f),
vec2(-0.3101082f, -0.849111f) );

float weightPoisson8[] = float[]( 0.0872441f,
0.016017f,
0.244504f,
0.328147f,
0.158607f,
0.00600838f,
0.117983f,
0.0414895f);

vec2 poisson12[] = vec2[]( vec2(0.7019781f, 0.4520128f),
vec2(0.8258755f, -0.5020158f),
vec2(0.2529077f, -0.07610185f),
vec2(0.942766f, -0.005521063f),
vec2(0.3767599f, 0.9109573f),
vec2(-0.20271f, 0.3307083f),
vec2(-0.4574393f, 0.8160738f),
vec2(0.2215029f, -0.6027451f),
vec2(-0.7218463f, 0.07683881f),
vec2(-0.3863325f, -0.364555f),
vec2(-0.3821487f, -0.8984079f),
vec2(-0.8921654f, -0.4421902f));

vec2 poisson16[] = vec2[]( vec2(0.04016172f, 0.6597927f),
vec2(0.2638538f, 0.1602272f),
vec2(-0.1504451f, -0.08892984f),
vec2(-0.3738313f, 0.4099245f),
vec2(0.4119262f, 0.866171f),
vec2(-0.5852049f, 0.7585394f),
vec2(-0.5958748f, -0.4137801f),
vec2(-0.02542462f, -0.5178167f),
vec2(0.526595f, -0.4582279f),
vec2(0.6193698f, -0.03387295f),
vec2(-0.9082987f, -0.1504123f),
vec2(0.7070741f, 0.3595641f),
vec2(-0.3845805f, -0.8846055f),
vec2(0.1423472f, -0.9382053f),
vec2(-0.9210126f, 0.2529431f),
vec2(-0.5567311f, 0.04821742f));

vec2 spiral[] = vec2[](vec2( -0.7071,  0.7071 ),
vec2( -0.0000, -0.8750 ),
vec2(  0.5303,  0.5303 ),
vec2( -0.6250, -0.0000 ),
vec2(  0.3536, -0.3536 ),
vec2( -0.0000,  0.3750 ),
vec2( -0.1768, -0.1768 ),
vec2(  0.1250,  0.0000 ),

vec2( -0.7071,  0.7071 ),
vec2( 0.8750, 0.0000 ),
vec2(  -0.5303,  -0.5303 ),
vec2( 0.0000, 0.6250 ),
vec2(  0.3536, -0.3536 ),
vec2( -0.3750,  0.0000 ),
vec2( 0.1768, 0.1768 ),
vec2( -0.0000, -0.1250 ));

#define PI 3.14159

vec3 getRandom(vec3 pos, float depth)
{
	vec2 rUV = UV * vec2(32447.21575577546,98791.124854564) * depth;
	return texture(randomTexture, rUV).rgb;
}

vec3 magic = vec3(0.06711056,0.00583715,52.9829189);
vec3 magic2 = vec3(0.03274566,0.0037545644,52.9534545);
vec2 PCFAO(float myDepth, vec2 lUV, vec2 randomDir, vec2 UV, float realDepth, float a, float b, float depth)
{
	float sh=0;
	float ao=0;
	float weight = 0;

	float noise = fract(magic2.z*fract(dot(gl_FragCoord.xy/*+vec2(0,iRand)*/+randomDir*0.0,magic2.xy)))*PI;
	vec2 rotRand = vec2(sin(noise),cos(noise));

	for(int i=0;i<8;i++)
	{
		vec2 poiss = spiral[i]*lDirPixel*8;
		vec2 poiss2;

		poiss2.x = poiss.x * rotRand.x - poiss.y * rotRand.y;
		poiss2.y = poiss.x * rotRand.y + poiss.y * rotRand.x;
		int ch = int(gl_FragCoord.x+gl_FragCoord.y+iRand)%2;
		if (ch == 1)
			poiss2 = vec2(poiss2.y,poiss2.x);

		vec3 r3 = vec3(0);//getRandom(vec3(0), depth);

		sh += texture(lDirDepthTexture,vec3(lUV + poiss2,myDepth))*0.125;

		vec2 nowAo = texture(aoTexture,UV + poiss2*80/realDepth).rg;
		float nowDepth = nowAo.g;

		if (abs(nowDepth-realDepth)<0.1*realDepth*0.2)
		{
			ao += nowAo.r;
			weight += 1;
		}
	}
	if (lUV.x<0 || lUV.x>1 || lUV.y<0 || lUV.y>1 || isnan(lUV.x))
		sh = 1;
	if (weight==0)
		return vec2(sh,1);
	return vec2(sh,ao/weight);
}

void main(){
	vec3 normal = texture( normalTexture, UV ).rgb*2-1;
	vec3 light = clamp( dirColor * dot( dirL, normal ), 0, 1 );
	vec3 albedo = texture( colorTexture, UV ).rgb;
	float depth = texture( depthTexture, UV ).r;
	float dotLN = clamp(dot( dirL, normal ),0,1);

	float n = zClip.x;
	float f = zClip.y;

	//dark evil magic
	float a = f / ( f - n );
	float b = f * n / ( n - f );
	float realDepth = b / (depth-a);
	float lerpDepth = (realDepth - n)/(f-n);
	vec3 pos = camPos + mix(wPos0, wPos1, lerpDepth);

	vec4 lPos = lDirMVP * vec4(pos + normal*0.18,1);
	//vec4 lPos = lDirMVP * vec4(pos + normal*0.8,1);
	vec2 lUV = (lPos.xy+1) * 0.5;

	float myDepth = (lPos.z+1)*0.5;
	float bias = -0.002+clamp( 0.0001*acos( dotLN ),0,1);
	bias = bias;

	vec3 random = getRandom(pos, depth);
	vec2 shao = PCFAO(myDepth+bias, lUV, random.gb, UV, realDepth, a, b, depth);

	//shao.y = 1;
	//shao.x = 1;
	//albedo = vec3(1,1,1);
	shao.y = 1-(1-shao.y)*max(0.8,(1-shao.x));
	vec3 newColor = (light*shao.x + ambient) * albedo * shao.y;
	color.rgb = newColor;
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float luma2 = dot(color.rgb,luma);
	float mult = (-1.33467*luma2 + 2.33467)*0.85;
	color *= mult;

	//color.rgb = vec3(texture(aoTexture,UV).r);
	//color.rgb = vec3(shao.y);
	//color.r = pow(depth,25);
	//color.r = (gl_FragCoord.x+gl_FragCoord.y)/(1980+1050);
}