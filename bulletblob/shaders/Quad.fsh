#version 330

in vec2 UV;

out vec3 color;

uniform sampler2D tex;
 
void main(){
	//vec2 UV2 = UV + vec2(sin(UV.x*3.14*5), sin(UV.y*3.14*5))*0.1;
	color = (texture( tex, UV ).rgb-0.5);
}