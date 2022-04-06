#version 430

in vec4 myPos;
in vec3 myColor;
in float a;
in float b;
in float c;

float r = 0.2;
out vec4 color;

void main(void)
{
	float linePosition = a*myPos.x - b * myPos.y - c;
	if(linePosition >= 0){
		color = vec4(abs(1-myColor.x),abs(1-myColor.y),myColor.z,1.0f);
	}
	else {
		color = vec4(myColor,1.0f);
	}
	if (myPos.z == 0.5f){
		color = vec4(1.0f,1.0f,1.0f,1.0f);
	}

}