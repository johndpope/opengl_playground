#pragma once
#include <glm.hpp>
#include <glew.h>
#include <camera.h>
#include <material.h>
#include <shader.h>
#include <shape.h>
#include <vector>

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
	BasicColorBox(const glm::vec3 color, const glm::vec3& size);
	BasicColorBox(const glm::vec3& size);

private:
	void generateCustomBox(float vertices[396], glm::vec3& size);
	void initBox(const GLuint& vao, const GLuint& vbo);
	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

	glm::vec3 m_color;
};

class ColorBox : public Box<ColorShader>
{
public:
	ColorBox(const glm::vec3 color, glm::vec3 size);
	ColorBox(const Material material, glm::vec3 size);

private:
	void generateCustomBox(float vertices[396], glm::vec3& size);
	void initBox(const GLuint& vao, const GLuint& vbo);
	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

	Material m_material;
	glm::vec3 m_color;
};

class MultiTextureBox : public Box<TextureShader>
{
public:
	MultiTextureBox(const std::string texturePaths[NUM_BOX_SIDES], glm::vec3 size);
	MultiTextureBox(const TextureMaterial textureMaterials[NUM_BOX_SIDES], glm::vec3 size);

private:
	void initBox(const GLuint& vao, const GLuint& vbo);
	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

	TextureMaterial m_textureMaterials[NUM_BOX_SIDES];
	GLuint m_textures[NUM_BOX_SIDES];
};