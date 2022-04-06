#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float offsetX;
uniform float offsetY;
uniform float x1;
uniform float x2;
uniform float y1;
uniform float y2;

out float a;
out float b;
out float c;
out vec3 myColor;
out vec4 myPos;

void main(void)
{
	if(aPos.z == 0.5f){
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}
	else {
		gl_Position = vec4(aPos.x+offsetX, aPos.y+offsetY, aPos.z, 1.0);
	}
	myPos = gl_Position;
	myColor = aColor;
	a = y2-y1;
	b = x2-x1;
	c = a*y2 - b*y1;
}