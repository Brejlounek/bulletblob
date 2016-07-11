#version 330

in vec2 UV;
in vec3 wPos0;
in vec3 wPos1;

layout(location = 0) out vec4 color;

uniform vec2 pixel;

uniform mat4 MVP;
uniform vec3 camPos;
uniform vec2 zClip;
uniform float zClipRatio;
uniform float iRand;

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

uniform sampler2D randomTexture;

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
float AO(vec2 UV, vec3 normal, float realDepth, float depth0, float radius, float a, float b, float n, float f, vec3 pos)
{
	float noise = fract((magic.z)*fract(dot(gl_FragCoord.xy,magic.xy)))*PI;
	vec2 rotRand = vec2(sin(noise),cos(noise));
	vec2 poiss0 = vec2(1,0);
	vec3 rvec;

	rvec.x = poiss0.x * rotRand.x - poiss0.y * rotRand.y;
	rvec.y = poiss0.x * rotRand.y + poiss0.y * rotRand.x;
	rvec.z = sin(cos(noise*99999));

	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

	float occlusion = 0;
	for(int i=0;i<8;i++)
	{
		vec3 randV = getRandom(vec3(0),depth0);
		randV.xy = 1+randV.xy*0.6;

		vec2 poiss = spiral[i]*randV.xy;
		vec3 sample = tbn * vec3(poiss,0);
		sample = sample * radius + pos;

		vec4 offset = vec4(sample, 1.0);
		offset = MVP * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;
		//offset.y=1-offset.y;

		float sampleDepth = texture(depthTexture, offset.xy+pixel*0.5).r;
		sampleDepth = b/(sampleDepth-a);

		float diff = abs(sampleDepth - offset.z);
		if (diff>realDepth/100)
		{
			float rangeCheck = smoothstep(0.0, 1.0, radius / diff * 0.5);
			float plus = step(sampleDepth, offset.z);
			occlusion += pow(plus,1)*rangeCheck/15;
		}
	}
	return pow(max(0,1-occlusion),3);
}

void main(){
	vec3 normal = texture( normalTexture, UV ).rgb*2-1;
	float depth = texture( depthTexture, UV ).r;

	float n = zClip.x;
	float f = zClip.y;

	//dark evil magic
	float a = f / ( f - n );
	float b = f * n / ( n - f );
	float realDepth = b / (depth-a);
	float lerpDepth = (realDepth - n)/(f-n);
	vec3 pos = camPos + mix(wPos0, wPos1, lerpDepth);

	float ao = AO(UV, normal, realDepth, depth, 3, a, b, n, f, pos);
	color.r = ao;
	color.g = realDepth;
}