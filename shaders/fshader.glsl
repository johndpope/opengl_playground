#version 440 core

in vec4 vFragColor;
in vec2 vFragTexCoord;

uniform sampler2D sTexture;

out vec4 vOutColor;

void main()
{
	vOutColor = texture(sTexture, vFragTexCoord);
}