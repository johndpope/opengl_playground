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

	void setAmbientColor(const glm::vec3& ambientColor);
	void setBufferColor(int stride, int offset);
    void setBufferColorNorm(int stride, int offset);
	void setBufferNormal(int stride, int offset);
	void setBufferPosition(int stride, int offset);
	void setBufferTextureCoord(int stride, int offset);
	void setDiffuseColor(const glm::vec3& diffuseColor);
	void setLightPosition(const glm::vec3& lightPosition);
	void setMaterial(const Material& material);
	void setModelView(const glm::mat4& modelView);
	void setModelViewProjection(const glm::mat4& modelViewProjection);
	void setShininess(const float shininess);
	void setSpecularColor(const glm::vec3& specularColor);
	void setView(const glm::mat4& view);

protected:
	void setBufferParameter(std::string name, int numAttribs, int stride, int offset);

	unsigned int m_pid;
};

class BasicColorShader : public ShaderBase
{
public:
	BasicColorShader() :
		ShaderBase("shader\\basicColorVertexShader.glsl", "shader\\basicColorFragmentShader.glsl") { }
};

class ColorShader : public ShaderBase
{
public:
	ColorShader() :
		ShaderBase("shader\\colorVertexShader.glsl", "shader\\colorFragmentShader.glsl") { }
};

class ColorMapShader : public ShaderBase
{
public:
	ColorMapShader() :
		ShaderBase("shader\\colorMapVertexShader.glsl", "shader\\colorMapFragmentShader.glsl") { }
};

class TextureShader : public ShaderBase
{
public:
	TextureShader() :
		ShaderBase("shader\\textureVertexShader.glsl", "shader\\textureFragmentShader.glsl") { }
};