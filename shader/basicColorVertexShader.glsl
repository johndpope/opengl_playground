#version 440 core //GLSL Version 4.4

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;

uniform mat4 mModelViewProj;

out vec4 vFragColor;

void main()
{
	vFragColor = vColor;
	gl_Position = mModelViewProj * vec4(vPosition, 1.0);
}