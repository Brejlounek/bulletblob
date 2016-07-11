#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
uniform mat4 MVP;
uniform mat4 MVPnoTransl;
uniform vec3 wiggle;

out vec3 fragmentColor;
out vec3 normalColor;
out vec3 worldPos;

void main(){
    vec4 v = vec4(vertexPosition_modelspace,1);
    gl_Position = MVP * v;
	fragmentColor=vertexColor;
	normalColor=vertexNormal;
	worldPos=vertexPosition_modelspace;
}