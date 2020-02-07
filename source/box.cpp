#include <ctime>
#include <box.h>

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

BasicColorBox::BasicColorBox(const glm::vec3 color, const glm::vec3& size)
	: Box(new BasicColorShader(), size),
	  m_color(color) { }

BasicColorBox::BasicColorBox(const glm::vec3& size)
	: Box(new BasicColorShader(), size),
	  m_color(glm::vec3(1.0f)) { }

void BasicColorBox::generateCustomBox(float vertices[396], glm::vec3& size)
{
	this->generateBox(vertices, size, &m_color);
}

void BasicColorBox::initBox(const GLuint& vao, const GLuint& vbo)
{
	m_shader->setBufferPosition(11, 0);
	m_shader->setBufferColor(11, 8);
}

void BasicColorBox::updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
	glm::mat4 modelViewProj = camera.projection() * camera.pose() * this->pose();
	m_shader->setModelViewProjection(modelViewProj);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

ColorBox::ColorBox(const glm::vec3 color, glm::vec3 size)
	: Box(new ColorShader(), size),
	  m_color(color)
{
	srand(time(NULL));

	m_material.shininess = rand() / (float)RAND_MAX;
	m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
	m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
}

ColorBox::ColorBox(const Material material, glm::vec3 size)
	: Box(new ColorShader(), size),
	  m_material(material),
	  m_color(glm::vec3(1.0f)) { }

void ColorBox::generateCustomBox(float vertices[396], glm::vec3& size)
{
	this->generateBox(vertices, size, &m_color);
}

void ColorBox::initBox(const GLuint& vao, const GLuint& vbo)
{
	m_shader->setBufferPosition(11, 0);
	m_shader->setBufferNormal(11, 5);
	m_shader->setBufferColor(11, 8);
}

void ColorBox::updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
	glm::mat4 modelView = camera.pose() * this->pose();
	glm::mat4 modelViewProj = camera.projection() * modelView;

	m_shader->setView(camera.pose());
	m_shader->setModelView(modelView);
	m_shader->setModelViewProjection(modelViewProj);
	m_shader->setLightPosition(light.translation());

	m_shader->setMaterial(m_material);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

MultiTextureBox::MultiTextureBox(const std::string texturePaths[NUM_BOX_SIDES], glm::vec3 size)
	: Box(new TextureShader(), size)
{
	srand(time(NULL));

	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		m_textureMaterials[i].texturePath = texturePaths[i];
		m_textureMaterials[i].material.shininess = rand() / (float)RAND_MAX;
		m_textureMaterials[i].material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
		m_textureMaterials[i].material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		m_textureMaterials[i].material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
	}
}

MultiTextureBox::MultiTextureBox(const TextureMaterial textureMaterials[NUM_BOX_SIDES], glm::vec3 size)
	: Box(new TextureShader(), size)
{
	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		m_textureMaterials[i] = textureMaterials[i];
	}
}

void MultiTextureBox::initBox(const GLuint& vao, const GLuint& vbo)
{
	m_shader->setBufferPosition(11, 0);
	m_shader->setBufferTextureCoord(11, 3);
	m_shader->setBufferNormal(11, 5);

	glGenTextures(NUM_BOX_SIDES, m_textures);
	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		unsigned char *data = stbi_load(m_textureMaterials[i].texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
	}
}

void MultiTextureBox::updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
	glm::mat4 modelView = camera.pose() * this->pose();
	glm::mat4 modelViewProj = camera.projection() * modelView;

	m_shader->setView(camera.pose());
	m_shader->setModelView(modelView);
	m_shader->setModelViewProjection(modelViewProj);
	m_shader->setLightPosition(light.translation());

	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		m_shader->setMaterial(m_textureMaterials[i].material);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glDrawArrays(GL_TRIANGLES, 6 * i, 6);
	}
}