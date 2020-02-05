#pragma once
#include <glm.hpp>
#include <glew.h>
#include <material.h>
#include <cshader.h>

class ShaderBase : public CShader
{
public:
	ShaderBase(std::string vertexShader, std::string fragmentShader) :
		CShader(vertexShader.c_str(), fragmentShader.c_str())
	{
		this->use();
		m_pid = this->getProgram();

		this->printAllAttributes();
	}

	void printAllAttributes()
	{
		const GLsizei bufferSize = 16;
		GLint size;
		GLenum type;
		GLchar name[bufferSize];
		GLsizei length;

		int count;
		glGetProgramiv(m_pid, GL_ACTIVE_ATTRIBUTES, &count);
		printf("Active Attributes: %d\n", count);

		for (int i = 0; i < count; i++)
		{
			glGetActiveAttrib(m_pid, (GLuint)i, bufferSize, &length, &size, &type, name);
			printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
		}

		printf("\n");
	}

	void setBufferPosition(int stride, int offset)
	{
		this->setBufferParameter("vPosition", 3, stride, offset);
	}

	void setModelView(const glm::mat4& modelView)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_pid, "mModelView"), 1, GL_FALSE, &modelView[0][0]);
	}

	void setModelViewProjection(const glm::mat4& modelViewProjection)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_pid, "mModelViewProj"), 1, GL_FALSE, &modelViewProjection[0][0]);
	}

protected:
	void setBufferParameter(std::string name, int numAttribs, int stride, int offset)
	{
		GLuint loc = glGetAttribLocation(m_pid, name.c_str());
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, numAttribs, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

		if (loc == (unsigned int)-1)
		{
			fprintf(stderr, "Failed to set buffer parameter: %s\n", name);
		}
	}

	unsigned int m_pid;
};

class LightingShader : public ShaderBase
{
public:
	LightingShader(std::string vertexShader, std::string fragmentShader) :
		ShaderBase(vertexShader, fragmentShader) { }

	void setModelViewNormal(const glm::mat4& modelViewNormal)
	{
		glUniformMatrix3fv(glGetUniformLocation(m_pid, "mModelViewNorm"), 1, GL_FALSE, &modelViewNormal[0][0]);
	}

	void setLightPosition(const glm::vec3& lightPosition)
	{
		glUniform3fv(glGetUniformLocation(m_pid, "vLightPosition"), 1, &lightPosition[0]);
	}

	void setBufferNormal(int stride, int offset)
	{
		this->setBufferParameter("vNormal", 3, stride, offset);
	}
};

class MaterialShader : public LightingShader
{
public:
	MaterialShader(std::string vertexShader, std::string fragmentShader) :
		LightingShader(vertexShader, fragmentShader) { }

	void setShininess(const float shininess)
	{
		glUniform1f(glGetUniformLocation(m_pid, "fShininess"), shininess);
	}

	void setAmbientColor(const glm::vec3& ambientColor)
	{
		glUniform3fv(glGetUniformLocation(m_pid, "vAmbientColor"), 1, &ambientColor[0]);
	}

	void setDiffuseColor(const glm::vec3& diffuseColor)
	{
		glUniform3fv(glGetUniformLocation(m_pid, "vDiffuseColor"), 1, &diffuseColor[0]);
	}

	void setSpecularColor(const glm::vec3& specularColor)
	{
		glUniform3fv(glGetUniformLocation(m_pid, "vSpecularColor"), 1, &specularColor[0]);
	}

	void setMaterial(const Material& material)
	{
		this->setShininess(material.shininess);
		this->setAmbientColor(material.ambientColor);
		this->setDiffuseColor(material.diffuseColor);
		this->setSpecularColor(material.specularColor);
	}
};

class BasicColorShader : public ShaderBase
{
public:
	BasicColorShader() :
		ShaderBase("shader\\basicColorVertexShader.glsl", "shader\\basicColorFragmentShader.glsl") { }

	void setBufferColor(int stride, int offset)
	{
		this->setBufferParameter("vColor", 3, stride, offset);
	}
};

class ColorShader : public MaterialShader
{
public:
	ColorShader() :
		MaterialShader("shader\\colorVertexShader.glsl", "shader\\colorFragmentShader.glsl") { }

	void setBufferColor(int stride, int offset)
	{
		this->setBufferParameter("vColor", 3, stride, offset);
	}
};

class TextureShader : public MaterialShader
{
public:
	TextureShader() :
		MaterialShader("shader\\textureVertexShader.glsl", "shader\\textureFragmentShader.glsl") { }

	void setBufferTextureCoord(int stride, int offset)
	{
		this->setBufferParameter("vTexCoord", 2, stride, offset);
	}
};