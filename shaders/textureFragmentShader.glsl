#version 440 core

layout(location = 0) out vec4 vFragColor;

uniform float fShininess;
uniform vec3 vAmbientColor;
uniform vec3 vDiffuseColor;
uniform vec3 vSpecularColor;

uniform vec3 vLightPosition;
uniform mat4 mModelView;

uniform sampler2D sDiffuse;
uniform sampler2D sSpecular;

smooth in vec3 vEyeSpaceNormal;
smooth in vec3 vEyeSpacePosition;
smooth in vec2 vFragTexCoord;

void main()
{
	vec3 vEyeSpaceLightPosition = (mModelView * vec4(vLightPosition, 1)).xyz;
	vec3 n = normalize(vEyeSpaceNormal); // Normal vector
	vec3 l = normalize(vEyeSpaceLightPosition - vEyeSpacePosition); // Ray from vertex to light
	vec3 r = normalize(2 * n * dot(n, l) - l); // Direction of reflected vector
	float diff = max(0, dot(n, l)); // Percentage of ray perpendicular to surface
	float spec = pow(max(0, dot(r, -vEyeSpacePosition)), fShininess); // Percentage of reflection in camera

	vec3 vAmbient  = vAmbientColor * vec3(texture(sDiffuse, vFragTexCoord));
	vec3 vDiffuse  = vDiffuseColor * diff * vec3(texture(sDiffuse, vFragTexCoord));
	vec3 vSpecular = vSpecularColor * spec * vec3(texture(sSpecular, vFragTexCoord));

	vFragColor = vec4(vAmbient + vDiffuse + vSpecular, 1.0);
	//vFragColor = vec4(vEyeSpaceNormal.x, vEyeSpaceNormal.y, vEyeSpaceNormal.z, 1.0);
}