#version 330

in vec2 UV;
in vec3 fragmentColor;
in vec3 normalColor;
in vec3 worldPos;

uniform sampler2D floorTex;
uniform vec3 col;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;
 
void main(){
	vec2 texUV = vec2(worldPos.x*0.03,worldPos.z*0.03);
	vec2 texUV2 = 1.0756*vec2(texUV.x*0.674711 - texUV.y*0.738082, texUV.x*0.738082 + texUV.y*0.674711);
	texUV2 += vec2(0.56465456,0.68556747);
	float dist = max(0,1 - max(0,length(worldPos)-100)/500.0);

	color = texture( floorTex, texUV ).rgb * col * 0.7 + texture( floorTex, texUV2 ).rgb * col * 0.7;
	color *= dist;
	normal = normalize(normalColor)*0.5 + 0.5;
}