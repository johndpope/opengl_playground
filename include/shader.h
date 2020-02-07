#pragma once
#include <glm.hpp>
#include <glew.h>
#include <material.h>
#include <cshader.h>

class ShaderBase : public CShader
{
public:
	ShaderBase(std::string vertexShader, std::string fragmentShader);

	void printActiveInfo();
	void setBufferPosition(int stride, int offset);
	void setView(const glm::mat4& view);
	void setModelView(const glm::mat4& modelView);
	void setModelViewProjection(const glm::mat4& modelViewProjection);

protected:
	void setBufferParameter(std::string name, int numAttribs, int stride, int offset);

	unsigned int m_pid;
};

class LightingShader : public ShaderBase
{
public:
	LightingShader(std::string vertexShader, std::string fragmentShader) :
		ShaderBase(vertexShader, fragmentShader) { }

	void setLightPosition(const glm::vec3& lightPosition);
	void setBufferNormal(int stride, int offset);
};

class MaterialShader : public LightingShader
{
public:
	MaterialShader(std::string vertexShader, std::string fragmentShader) :
		LightingShader(vertexShader, fragmentShader) { }

	void setShininess(const float shininess);
	void setAmbientColor(const glm::vec3& ambientColor);
	void setDiffuseColor(const glm::vec3& diffuseColor);
	void setSpecularColor(const glm::vec3& specularColor);
	void setMaterial(const Material& material);
};

class BasicColorShader : public ShaderBase
{
public:
	BasicColorShader() :
		ShaderBase("shader\\basicColorVertexShader.glsl", "shader\\basicColorFragmentShader.glsl") { }

	void setBufferColor(int stride, int offset);
};

class ColorShader : public MaterialShader
{
public:
	ColorShader() :
		MaterialShader("shader\\colorVertexShader.glsl", "shader\\colorFragmentShader.glsl") { }

	void setBufferColor(int stride, int offset);
};

class TextureShader : public MaterialShader
{
public:
	TextureShader() :
		MaterialShader("shader\\textureVertexShader.glsl", "shader\\textureFragmentShader.glsl") { }

	void setBufferTextureCoord(int stride, int offset);
};