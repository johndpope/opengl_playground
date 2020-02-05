#version 440 core //GLSL Version 4.4

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

uniform mat4 mModelViewProj;

out vec4 vFragColor;

void main()
{
	vFragColor = vec4(vColor, 1.0);
	gl_Position = mModelViewProj * vec4(vPosition, 1.0);
}