#version 330

in vec2 UV;

out vec4 color;

uniform vec3 pixel;
uniform vec3 wiggle;
uniform float iRand;

uniform sampler2D tex;
uniform sampler2D prev0;
uniform sampler2D velTexture;

#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#define FXAA_SPAN_MAX     8.0

bool fitsnot(vec3 what,vec3 min,vec3 max)
{
	return (what.x<min.x || what.x>max.x
	 || what.y<min.y || what.y>max.y
	 || what.z<min.z || what.z>max.z);
}

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

vec3 magic = vec3(0.06711056,0.00583715,52.9829189);
vec3 lumaDot = vec3(0.2126, 0.7152, 0.0722);

void main(){

	vec2 UV2 = UV;
	vec4 nowTexBig = texture( tex, UV2 ).rgba;
	vec3 nowTex = nowTexBig.rgb;

	vec2 velocity = texture( velTexture, UV2 ).rg;
	/*color.b = 0;
	color.rg = velocity;	
	return;*/

	velocity *= 2;
	velocity -= 1;
	velocity /=3;
	vec2 sgn = sign(velocity);
	velocity = pow(velocity, vec2(2.0,2.0)) * sgn;
	//velocity*=16;
	vec2 newUV = UV2-velocity;
	if (newUV.x<0 || newUV.y<0)
	{
		color.rgb = nowTex;
		color.a = 0.5;
		return;
	}
	vec2 velUV = UV2-velocity;
	vec4 prevTexBig = texture( prev0, velUV ).rgba;
	vec3 prevTex = prevTexBig.rgb;

	vec3 nowTexE = texture( tex, UV2+vec2(pixel.x,0) ).rgb;
	vec3 nowTexW = texture( tex, UV2+vec2(-pixel.x,0) ).rgb;
	vec3 nowTexS = texture( tex, UV2+vec2(0,pixel.y) ).rgb;
	vec3 nowTexN = texture( tex, UV2+vec2(0,-pixel.y) ).rgb;

	/*float dX = abs(dot(nowTexN-nowTex, lumaDot));
	float dY = abs(dot(nowTexW-nowTex, lumaDot));
	float dX2 = abs(dot(nowTexS-nowTex, lumaDot));
	float dY2 = abs(dot(nowTexE-nowTex, lumaDot));

	dX = max(dX,dX2);
	dY = max(dY,dY2);

	if (max(dX,dY)<18.0/255)
	{
		color.rgb = nowTex;
		return;
	}*/

	vec3 nowTexNW = texture( tex, UV2+vec2(-pixel.x,-pixel.y) ).rgb;
	vec3 nowTexNE = texture( tex, UV2+vec2(pixel.x,-pixel.y) ).rgb;
	vec3 nowTexSE = texture( tex, UV2+vec2(pixel.x,pixel.y) ).rgb;
	vec3 nowTexSW = texture( tex, UV2+vec2(-pixel.x,pixel.y) ).rgb;

	vec3 minRange = min(nowTexE, min(nowTexW, min(nowTexS, min(nowTexN,min(nowTexNW, min(nowTexNE, min(nowTexSE,min(nowTexSW,nowTex))))))));
	vec3 maxRange = max(nowTexE, max(nowTexW, max(nowTexS, max(nowTexN,max(nowTexNW, max(nowTexNE, max(nowTexSE,max(nowTexSW,nowTex))))))));

	/*nowTex = vec3(0);
	for(int i=0;i<16;i++)
	{
		vec2 poiss = spiral[i]*pixel.xy*8;
		vec2 poiss2;

		float noise = fract(magic.z*fract(dot(gl_FragCoord.xy+vec2(iRand,0),magic.xy)))*PI;
		vec2 rotRand = vec2(sin(noise),cos(noise));

		poiss2.x = poiss.x * rotRand.x - poiss.y * rotRand.y;
		poiss2.y = poiss.x * rotRand.y + poiss.y * rotRand.x;
		int ch = int(gl_FragCoord.x+gl_FragCoord.y)%2;
		if (ch == 1)
			poiss2*=mat2(0,1,1,0);

		nowTex += texture(tex,UV2 + poiss2 + wiggle.xy*0.5).rgb*0.0625;
	}*/

	/*if (fitsnot(prevTex,minRange,maxRange))
	{
		color.rgb = nowTex;
		return;
	}*/

	float ratio = 0.03;

	prevTex=clamp(prevTex,minRange,maxRange);

	color.rgb = nowTex*ratio + prevTex*(1-ratio);

	float diff = abs(dot(nowTex-prevTex, lumaDot));
	color.a = prevTexBig.a*0.5 + diff/dot(nowTex, lumaDot);
	//color.r = prevTexBig.a;

	//color.rgb = nowTex;
}