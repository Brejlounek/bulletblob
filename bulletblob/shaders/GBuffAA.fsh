#version 330

in vec3 fragmentColor;
in vec3 normalColor;
in vec3 worldPos;

in vec4 newPos;
in vec4 oldPos;

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 velOut;

uniform mat4 MVP;
uniform mat4 MVPprev;
uniform vec3 wiggle;
uniform vec3 lastWiggle;
 
void main(){
	color.rgb = fragmentColor;
	normal = normalize(normalColor)*0.5 + 0.5;

	float newW = newPos.w*1;
	float oldW = oldPos.w*1;

	velOut.rg = newPos.xy/newW-oldPos.xy/oldW;
	velOut.rg *= 0.5;

	color.a = abs(velOut.r)*100 + abs(velOut.g)*100;

	vec2 sgn = sign( velOut.rg );
	//velOut.rg/=16;
	velOut.rg = pow(abs( velOut.rg ), vec2(0.5,0.5)) * sgn;
	velOut.rg*=3;
	velOut.rg += 1;
	velOut.rg *= 0.5;
}