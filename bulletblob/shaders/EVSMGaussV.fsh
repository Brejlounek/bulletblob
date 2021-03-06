#version 330

in vec2 UV;

out vec2 color;

uniform sampler2D tex;
uniform sampler2D prev0;
uniform sampler2D vel;

uniform vec2 pixel;

float gaussWeights7[] = float[](0.038735, 0.113085, 0.215007, 0.266346, 0.215007, 0.113085, 0.038735);
float gaussWeights9[] = float[](0.028532, 0.067234, 0.124009, 0.179044, 0.20236, 0.179044, 0.124009, 0.067234, 0.028532);

void main(){
	vec2 moment;
	moment = vec2(0,0);
	for(int i=-4;i<5;i++)
	{
		vec2 depth = texture( tex, UV + vec2(0, i*pixel.y)).rg;
		moment += depth * gaussWeights9[i+4];
	}
	color.rg = moment;
}