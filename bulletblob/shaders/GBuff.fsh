#version 330

in vec3 fragmentColor;
in vec3 normalColor;
in vec3 worldPos;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;
 
void main(){
	color = fragmentColor;
	normal = normalize(normalColor)*0.5 + 0.5;
}