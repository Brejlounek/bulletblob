#version 330

in vec2 UV;
in vec3 wPos;

layout(location = 0) out vec4 color;

uniform vec3 ambient;
uniform vec3 dirL;
uniform vec3 dirColor;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
 
void main(){
	vec3 normal = texture( normalTexture, UV ).rgb*2-1;
	vec3 light = clamp( dirColor * dot( dirL, normal ), 0, 1 ) + ambient;
	vec3 albedo = texture( colorTexture, UV ).rgb;
	float depth = texture( depthTexture, UV ).r;
	vec3 newColor = light * albedo;
	color.rgb = newColor;
}