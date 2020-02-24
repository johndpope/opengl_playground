#version 440 core //GLSL Version 4.4

layout(location = 0) in vec3 vPosition;
layout(location = 1) in float fColorNorm;

uniform mat4 mModelViewProj;

smooth out float fFragColorNorm;

void main()
{
	gl_Position = mModelViewProj * vec4(vPosition, 1.0);

	fFragColorNorm = fColorNorm;
}