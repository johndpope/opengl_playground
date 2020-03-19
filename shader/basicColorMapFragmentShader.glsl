#version 440 core

uniform sampler1D sColor;

smooth in float fFragColorNorm;

out vec4 vOutColor;

void main()
{
	vOutColor = vec4(texture(sColor, fFragColorNorm));
}