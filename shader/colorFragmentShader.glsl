#version 440 core

uniform float fShininess;
uniform vec3 vAmbientColor;
uniform vec3 vDiffuseColor;
uniform vec3 vSpecularColor;

uniform vec3 vLightPosition;
uniform mat4 mModelView;

smooth in vec3 vEyeSpaceNormal;
smooth in vec3 vEyeSpacePosition;
smooth in vec3 vEyeSpaceLightPosition;
smooth in vec3 vFragColor;

out vec4 vOutColor;

void main()
{
	vec3 n = normalize(vEyeSpaceNormal); // Normal vector
	vec3 l = normalize(vEyeSpaceLightPosition - vEyeSpacePosition); // Ray from vertex to light
	vec3 r = reflect(-l, n);
	float diff = max(0, dot(n, l)); // Percentage of ray perpendicular to surface
	float spec = pow(max(0, dot(normalize(-vEyeSpacePosition), r)), fShininess); // Percentage of reflection in camera

	vec3 vAmbient  = vAmbientColor * vFragColor;
	vec3 vDiffuse  = vDiffuseColor * diff * vFragColor;
	vec3 vSpecular = vSpecularColor * spec * vFragColor;

	vOutColor = vec4(vAmbient + vDiffuse + vSpecular, 1.0);
}