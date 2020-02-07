#pragma once
#include <glm.hpp>
#include <glew.h>
#include <glm\gtc\matrix_inverse.hpp>
#include <camera.h>
#include <material.h>
#include <shader.h>
#include <shape.h>
#include <ctime>
#include <vector>

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#define NUM_BOX_SIDES 6
template <class Shader>
class Box : public Shape<Shader>
{
static_assert(std::is_base_of<ShaderBase, Shader>::value, "Shader must derive from ShaderBase");

public:
	Box(Shader* shader, const glm::vec3& size) :
		Shape<Shader>(shader),
		m_size(size) { }

	Box(Shader* shader) :
		Shape<Shader>(shader)
	{
		m_size = glm::vec3(1.0f);
	}

protected:
	virtual ~Box() = default;

	virtual void initBox(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light) = 0;
	virtual void generateCustomBox(float vertices[396], glm::vec3& size)
	{
		this->generateBox(vertices, size);
	}

	void generateBox(float vertices[396], glm::vec3& size, glm::vec3* color = nullptr)
	{
		glm::vec3 defaultColor = glm::vec3(1.0f);
		if (color == nullptr)
		{
			color = &defaultColor;
		}

		float minX = -size.x / 2;
		float maxX = size.x / 2;
		float minY = -size.y / 2;
		float maxY = size.y / 2;
		float minZ = -size.z / 2;
		float maxZ = size.z / 2;

		glm::vec3 v0 = glm::vec3(minX, minY, maxZ);
		glm::vec3 v1 = glm::vec3(maxX, minY, maxZ);
		glm::vec3 v2 = glm::vec3(maxX, maxY, maxZ);
		glm::vec3 v3 = glm::vec3(minX, maxY, maxZ);
		glm::vec3 v4 = glm::vec3(maxX, minY, minZ);
		glm::vec3 v5 = glm::vec3(minX, minY, minZ);
		glm::vec3 v6 = glm::vec3(minX, maxY, minZ);
		glm::vec3 v7 = glm::vec3(maxX, maxY, minZ);

		glm::vec3 posZ[4] = { v0, v1, v2, v3 };
		glm::vec3 posY[4] = { v3, v2, v7, v6 };
		glm::vec3 posX[4] = { v1, v4, v7, v2 };
		glm::vec3 negZ[4] = { v4, v5, v6, v7 };
		glm::vec3 negY[4] = { v0, v5, v4, v1 };
		glm::vec3 negX[4] = { v5, v0, v3, v6 };

		glm::vec3* faces[6] = { posX, posY, posZ, negX, negY, negZ };
		int sequence[6] = { 0, 1, 2, 0, 2, 3 };

		// Iterate over faces
		for (int i = 0; i < 6; i++)
		{
			// Calculate face normal
			glm::vec3 a = faces[i][2] - faces[i][0];
			glm::vec3 b = faces[i][1] - faces[i][0];
			glm::vec3 n = glm::normalize(glm::cross(b, a));

			// Iterate over vertices
			for (int k = 0; k < 6; k++)
			{
				int m = sequence[k];
				int o = (i * 6 + k) * 11;

				// Vertex coordinate
				vertices[o + 0] = faces[i][m].x;
				vertices[o + 1] = faces[i][m].y;
				vertices[o + 2] = faces[i][m].z;

				// Vertex texture coordinate
				vertices[o + 3] = ((m == 0) || (m == 3)) ? 0.0f : 1.0f;
				vertices[o + 4] = ((m == 0) || (m == 1)) ? 0.0f : 1.0f;

				// Vertex normal
				vertices[o + 5] = n.x;
				vertices[o + 6] = n.y;
				vertices[o + 7] = n.z;

				// Vertex color
				vertices[o + 8] = color->x;
				vertices[o + 9] = color->y;
				vertices[o + 10] = color->z;
			}
		}
	}

private:
	void initShape(const GLuint& vao, const GLuint& vbo)
	{
		float vertices[396];
		this->generateCustomBox(vertices, m_size);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		this->initBox(vao, vbo);
	}

	void updateShape(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
	{
		this->updateBox(totalTime, frameTime, camera, light);
	}

	glm::vec3 m_size;
};

class BasicColorBox : public Box<BasicColorShader>
{
public:
	BasicColorBox(const glm::vec3 color, const glm::vec3& size)
		: Box(new BasicColorShader(), size),
		  m_color(color) { }

	BasicColorBox(const glm::vec3& size)
		: Box(new BasicColorShader(), size),
		m_color(glm::vec3(1.0f)) { }

private:
	void generateCustomBox(float vertices[396], glm::vec3& size)
	{
		this->generateBox(vertices, size, &m_color);
	}

	void initBox(const GLuint& vao, const GLuint& vbo)
	{
		m_shader->setBufferPosition(11, 0);
		m_shader->setBufferColor(11, 8);
	}

	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
	{
		glm::mat4 modelViewProj = camera.projection() * camera.pose() * this->pose();
		m_shader->setModelViewProjection(modelViewProj);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glm::vec3 m_color;
};

class ColorBox : public Box<ColorShader>
{
public:
	ColorBox(const glm::vec3 color, glm::vec3 size)
		: Box(new ColorShader(), size),
		  m_color(color)
	{
		srand(time(NULL));

		m_material.shininess = rand() / (float)RAND_MAX;
		m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);// glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
	}

	ColorBox(const Material material, glm::vec3 size)
		: Box(new ColorShader(), size),
		  m_material(material),
		  m_color(glm::vec3(1.0f)) { }

private:
	void generateCustomBox(float vertices[396], glm::vec3& size)
	{
		this->generateBox(vertices, size, &m_color);
	}

	void initBox(const GLuint& vao, const GLuint& vbo)
	{
		m_shader->setBufferPosition(11, 0);
		m_shader->setBufferNormal(11, 5);
		m_shader->setBufferColor(11, 8);
	}

	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
	{
		glm::mat4 modelView = camera.pose() * this->pose();
		glm::mat4 modelViewProj = camera.projection() * modelView;
		glm::mat3 modelViewNorm = glm::mat3(glm::extractMatrixRotation(modelView));

		m_shader->setView(camera.pose());
		m_shader->setModelView(modelView);
		m_shader->setModelViewProjection(modelViewProj);
		m_shader->setLightPosition(light.translation());

		m_shader->setMaterial(m_material);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	Material m_material;
	glm::vec3 m_color;
};

class UniqueTextureBox : public Box<TextureShader>
{
public:
	UniqueTextureBox(const std::string texturePaths[NUM_BOX_SIDES], glm::vec3 size)
		: Box(new TextureShader(), size)
	{
		srand(time(NULL));

		for (int i = 0; i < NUM_BOX_SIDES; i++)
		{
			m_textureMaterials[i].texturePath = texturePaths[i];
			m_textureMaterials[i].material.shininess = rand() / (float)RAND_MAX;
			m_textureMaterials[i].material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);// glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
			m_textureMaterials[i].material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
			m_textureMaterials[i].material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		}
	}

	UniqueTextureBox(const TextureMaterial textureMaterials[NUM_BOX_SIDES], glm::vec3 size)
		: Box(new TextureShader(), size)
	{
		for (int i = 0; i < NUM_BOX_SIDES; i++)
		{
			m_textureMaterials[i] = textureMaterials[i];
		}
	}

private:
	void initBox(const GLuint& vao, const GLuint& vbo)
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

	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
	{
		glm::mat4 modelView = camera.pose() * this->pose();
		glm::mat4 modelViewProj = camera.projection() * modelView;
		glm::mat3 modelViewNorm = glm::mat3(glm::extractMatrixRotation(modelView));

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

	TextureMaterial m_textureMaterials[NUM_BOX_SIDES];
	GLuint m_textures[NUM_BOX_SIDES];
};