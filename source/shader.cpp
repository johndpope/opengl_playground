#pragma once
#include <shader.h>

ShaderBase::ShaderBase(std::string vertexShader, std::string fragmentShader) :
	CShader(vertexShader.c_str(), fragmentShader.c_str())
{
	this->use();
	m_pid = this->getProgram();

	this->printActiveInfo();
}

void ShaderBase::printActiveInfo()
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

	glGetProgramiv(m_pid, GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for (int i = 0; i < count; i++)
	{
		glGetActiveUniform(m_pid, (GLuint)i, bufferSize, &length, &size, &type, name);

		printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
	}

	printf("\n");
}

void ShaderBase::setBufferPosition(int stride, int offset)
{
	this->setBufferParameter("vPosition", 3, stride, offset);
}

void ShaderBase::setView(const glm::mat4& view)
{
	glUniformMatrix4fv(glGetUniformLocation(m_pid, "mView"), 1, GL_FALSE, &view[0][0]);
}

void ShaderBase::setModelView(const glm::mat4& modelView)
{
	glUniformMatrix4fv(glGetUniformLocation(m_pid, "mModelView"), 1, GL_FALSE, &modelView[0][0]);
}

void ShaderBase::setModelViewProjection(const glm::mat4& modelViewProjection)
{
	glUniformMatrix4fv(glGetUniformLocation(m_pid, "mModelViewProj"), 1, GL_FALSE, &modelViewProjection[0][0]);
}

void ShaderBase::setBufferParameter(std::string name, int numAttribs, int stride, int offset)
{
	GLuint loc = glGetAttribLocation(m_pid, name.c_str());
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, numAttribs, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	if (loc == (unsigned int)-1)
	{
		fprintf(stderr, "Failed to set buffer parameter: %s\n", name.c_str());
	}
}

void ShaderBase::setLightPosition(const glm::vec3& lightPosition)
{
	glUniform3fv(glGetUniformLocation(m_pid, "vLightPosition"), 1, &lightPosition[0]);
}

void ShaderBase::setBufferNormal(int stride, int offset)
{
	this->setBufferParameter("vNormal", 3, stride, offset);
}

void ShaderBase::setShininess(const float shininess)
{
	glUniform1f(glGetUniformLocation(m_pid, "fShininess"), shininess);
}

void ShaderBase::setAmbientColor(const glm::vec3& ambientColor)
{
	glUniform3fv(glGetUniformLocation(m_pid, "vAmbientColor"), 1, &ambientColor[0]);
}

void ShaderBase::setDiffuseColor(const glm::vec3& diffuseColor)
{
	glUniform3fv(glGetUniformLocation(m_pid, "vDiffuseColor"), 1, &diffuseColor[0]);
}

void ShaderBase::setSpecularColor(const glm::vec3& specularColor)
{
	glUniform3fv(glGetUniformLocation(m_pid, "vSpecularColor"), 1, &specularColor[0]);
}

void ShaderBase::setMaterial(const Material& material)
{
	this->setShininess(material.shininess);
	this->setAmbientColor(material.ambientColor);
	this->setDiffuseColor(material.diffuseColor);
	this->setSpecularColor(material.specularColor);
}

void ShaderBase::setBufferColor(int stride, int offset)
{
	this->setBufferParameter("vColor", 4, stride, offset);
}

void ShaderBase::setBufferTextureCoord(int stride, int offset)
{
	this->setBufferParameter("vTexCoord", 2, stride, offset);
}