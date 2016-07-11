#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

void main(){
    vec4 v = vec4(vertexPosition_modelspace,1);
    gl_Position = v;
	UV=(vertexPosition_modelspace.xy+1)*0.5;
}