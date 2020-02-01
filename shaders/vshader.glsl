#version 440 core //GLSL Version 4.4

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vTexCoord;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;

out vec4 vFragColor;
out vec2 vFragTexCoord;

void main()
{
	vFragColor = vec4(vColor, 1.0);
	gl_Position =  mProj*mView*mModel*vec4(vPosition,1.0); // Note order of multiplications: apply Model transform first, then View, then Proj
	vFragTexCoord = vTexCoord;
}