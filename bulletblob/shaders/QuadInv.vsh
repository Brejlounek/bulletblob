#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 invMat;
uniform vec3 camPos;

out vec2 UV;
out vec3 wPos0;
out vec3 wPos1;

void main(){
    vec4 v = vec4(vertexPosition_modelspace,1);
    gl_Position = v;
	UV=(vertexPosition_modelspace.xy+1)*0.5;

	vec4 rPos0 = invMat * vec4(vertexPosition_modelspace.xy,0,1);
	vec4 rPos1 = invMat * vec4(vertexPosition_modelspace.xy,1,1);
	wPos0 = rPos0.xyz/rPos0.w;
	wPos1 = rPos1.xyz/rPos1.w;
}