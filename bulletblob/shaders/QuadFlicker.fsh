#version 330

in vec2 UV;

out vec3 color;

uniform vec3 rand;
uniform vec3 pixel;
uniform vec3 sw;
uniform sampler2D nowTex;
uniform sampler2D prevTex;
 
 float rfunc(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	vec4 nowTex2 = texture( nowTex, UV ).rgba;
	vec4 prevTex2 = texture( prevTex, UV ).rgba;

	if (sw.x>0.5)
	{
		color=nowTex2.rgb;
		return;
	}

	float Y0 = nowTex2.r*0.2989 + nowTex2.g*0.5870 + nowTex2.b*0.1140;
	float Y1 = prevTex2.r*0.2989 + prevTex2.g*0.5870 + prevTex2.b*0.1140;

	float r0=rfunc(rand.xy*68721+UV*324564)*2-1;
	float r1=rfunc(rand.xy*354541+UV*897654)*2-1;
	if (abs(Y0-Y1)>0.3)
		color=texture( nowTex, UV+3*pixel.xy*vec2(r0,r1) ).rgb;
	else
		color=nowTex2.rgb;
	return;
}