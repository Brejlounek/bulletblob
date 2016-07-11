#version 330

in vec2 UV;

out vec3 color;

uniform sampler2D tex;
uniform vec2 pixel;
 
void main(){
	vec3 col0=texture( tex, UV+vec2(-pixel.x,-pixel.y) ).rgb;
	vec3 col1=texture( tex, UV+vec2(0,-pixel.y) ).rgb;
	vec3 col2=texture( tex, UV+vec2(pixel.x,-pixel.y) ).rgb;
	vec3 col3=texture( tex, UV+vec2(-pixel.x,0) ).rgb;
	vec3 col4=texture( tex, UV+vec2(0,0) ).rgb;
	vec3 col5=texture( tex, UV+vec2(pixel.x,0) ).rgb;
	vec3 col6=texture( tex, UV+vec2(-pixel.x,pixel.y) ).rgb;
	vec3 col7=texture( tex, UV+vec2(0,pixel.y) ).rgb;
	vec3 col8=texture( tex, UV+vec2(pixel.x,pixel.y) ).rgb;

	color = col0 + col1 + col2 + col3 + col4 + col5 + col6 + col7 + col8;
	color /= 9;
}