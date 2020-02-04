#version 440 core //GLSL Version 4.4

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

uniform mat4 mModelView;
uniform mat4 mModelViewProj;
uniform mat3 mModelViewNorm;

smooth out vec3 vEyeSpaceNormal;
smooth out vec3 vEyeSpacePosition;

out vec2 vFragTexCoord;

void main()
{
	vec4 hvPosition = vec4(vPosition, 1.0);
	vEyeSpacePosition = (mModelView * hvPosition).xyz;
	vEyeSpaceNormal = mModelViewNorm * vNormal;
	gl_Position = mModelViewProj * hvPosition;

	vFragTexCoord = vTexCoord;
}