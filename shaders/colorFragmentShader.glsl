#version 440 core

layout(location = 0) out vec4 vLightFragColor;

uniform float fShininess;
uniform vec3 vAmbientColor;
uniform vec3 vDiffuseColor;
uniform vec3 vSpecularColor;

uniform vec3 vLightPosition;
uniform mat4 mModelView;

smooth in vec3 vEyeSpaceNormal;
smooth in vec3 vEyeSpacePosition;
smooth in vec3 vFragColor;

void main()
{
	vec3 vEyeSpaceLightPosition = (mModelView * vec4(vLightPosition, 1)).xyz;
	vec3 n = normalize(vEyeSpaceNormal); // Normal vector
	vec3 l = normalize(vEyeSpaceLightPosition - vEyeSpacePosition); // Ray from vertex to light
	vec3 r = normalize(2 * n * dot(n, l) - l); // Direction of reflected vector
	float diff = max(0, dot(n, l)); // Percentage of ray perpendicular to surface
	float spec = pow(max(0, dot(r, -vEyeSpacePosition)), fShininess); // Percentage of reflection in camera

	vec3 vAmbient  = vAmbientColor * vFragColor;
	vec3 vDiffuse  = vDiffuseColor * diff * vFragColor;
	vec3 vSpecular = vSpecularColor * spec * vFragColor;

	vLightFragColor = vec4(vAmbient + vDiffuse + vSpecular, 1.0);
}