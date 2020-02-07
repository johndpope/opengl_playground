#version 440 core //GLSL Version 4.4

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

uniform vec3 vLightPosition;

uniform mat4 mView;
uniform mat4 mModelView;
uniform mat4 mModelViewProj;

smooth out vec3 vEyeSpaceNormal;
smooth out vec3 vEyeSpacePosition;
smooth out vec3 vEyeSpaceLightPosition;
smooth out vec2 vFragTexCoord;

void main()
{
	vec4 hvPosition = vec4(vPosition, 1.0);
	gl_Position = mModelViewProj * hvPosition;

	vEyeSpacePosition = (mModelView * hvPosition).xyz;
	vEyeSpaceNormal = mat3(transpose(inverse(mModelView))) * vNormal;
	vEyeSpaceLightPosition = (mView * vec4(vLightPosition, 1)).xyz;

	vFragTexCoord = vTexCoord;
}