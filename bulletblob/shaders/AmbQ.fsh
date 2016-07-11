#version 330

in vec2 UV;

out vec3 color;

uniform vec3 ambient;
uniform sampler2D colorTexture;
//uniform sampler2D normalTexture;
 
void main(){
	color = texture( colorTexture, UV ).rgb*ambient;
}