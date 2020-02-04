#pragma once
#include <glm.hpp>
#include <glew.h>
#include <glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_access.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtx\matrix_decompose.hpp>
#include <glm\gtx\matrix_interpolation.hpp>
#include <shaderWrapper.h>

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

struct Material
{
	float shininess;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
};

struct TextureMaterial
{
	std::string texturePath;
	Material material;
};

class ShaderBase : public CShader
{
public:
	ShaderBase(std::string vertexShader, std::string fragmentShader) :
		CShader(vertexShader.c_str(), fragmentShader.c_str())
	{
		this->use();
		m_pid = this->getProgram();
	}

	void setBufferPosition(int stride, int offset)
	{
		this->setBufferParameter("vPosition", 3, stride, offset);
	}

protected:
	void setBufferParameter(std::string name, int numAttribs, int stride, int offset)
	{
		GLuint loc = glGetAttribLocation(m_pid, name.c_str());
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, numAttribs, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float))); // use interleaved stride
	}

	unsigned int m_pid;
};

class LightingShader : public ShaderBase
{
public:
	LightingShader(std::string vertexShader, std::string fragmentShader) :
		ShaderBase(vertexShader, fragmentShader) { }

	void setModelView(const glm::mat4& modelView)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_pid, "mModelView"), 1, GL_FALSE, &modelView[0][0]);
	}

	void setModelViewProjection(const glm::mat4& modelViewProjection)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_pid, "mModelViewProj"), 1, GL_FALSE, &modelViewProjection[0][0]);
	}

	void setModelViewNormal(const glm::mat4& modelViewNormal)
	{
		glUniformMatrix3fv(glGetUniformLocation(m_pid, "mModelViewNorm"), 1, GL_FALSE, &modelViewNormal[0][0]);
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

class ColorShader : public MaterialShader
{
public:
	ColorShader() :
		MaterialShader("shaders\\colorVertexShader.glsl", "shaders\\colorFragmentShader.glsl") { }

	void setBufferColor(int stride, int offset)
	{
		this->setBufferParameter("vColor", 3, stride, offset);
	}
};

class TextureShader : public MaterialShader
{
public:
	TextureShader() :
		MaterialShader("shaders\\textureVertexShader.glsl", "shaders\\textureFragmentShader.glsl") { }

	void setBufferTextureCoord(int stride, int offset)
	{
		this->setBufferParameter("vTexCoord", 2, stride, offset);
	}
};

class UpdatableObject
{
public:
	UpdatableObject() { };

	void init()
	{
		if (m_init)
		{
			return;
		}

		m_startTime = glfwGetTime();
		m_lastTotalTime = 0;
		m_totalTime = 0;

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		this->initUpdatable(m_vao, m_vbo);
		glBindVertexArray(0);

		m_init = true;
	}

	void update(const glm::mat4& projection, const glm::mat4& view)
	{
		m_totalTime = glfwGetTime() - m_startTime;
		float elapsedTime = m_totalTime - m_lastTotalTime;

		glBindVertexArray(m_vao);
		this->updateUpdatable(m_totalTime, elapsedTime, projection, view);
		glBindVertexArray(0);

		m_lastTotalTime = m_totalTime;
	}

protected:
	virtual ~UpdatableObject() = default;

	virtual void initUpdatable(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateUpdatable(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view) = 0;

private:
	CShader* m_shader;
	GLuint m_vao;
	GLuint m_vbo;
	float m_lastTotalTime;
	float m_startTime;
	float m_totalTime;
	bool m_init;
};

class MovableObject : public UpdatableObject
{
public:
	MovableObject() = default;

	const glm::mat4& pose()
	{
		return m_transform;
	}

	const glm::vec3 translation()
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, quaternion, translation, skew, perspective);

		return translation;
	}

	const glm::mat4 rotation()
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, quaternion, translation, skew, perspective);

		return glm::mat4_cast(quaternion);
	}

	void scale(const glm::vec3& scale)
	{
		m_transform = glm::scale(m_transform, scale);
	}

	void rotate(const glm::vec3& axis, const float& angleInDeg)
	{
		m_transform = glm::rotate(m_transform, (float)glm::radians(angleInDeg), glm::normalize(axis));
	}

	void translate(const glm::vec3& translation)
	{
		m_transform = glm::translate(m_transform, translation);
	}

	void scaleTo(const glm::vec3& targetScale, const float& duration = -1.0f)
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, quaternion, translation, skew, perspective);

		glm::mat4 transform = this->computeTransform(targetScale, glm::mat4_cast(quaternion), translation);

		this->transformTo(transform, duration);
	}

	void rotateTo(const glm::vec3& targetAxis, const float& targetAngleInDeg, const float& duration = -1.0f)
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, quaternion, translation, skew, perspective);

		glm::mat4 rotation = glm::rotate(glm::mat4(), (float)glm::radians(targetAngleInDeg), glm::normalize(targetAxis));
		glm::mat4 transform = this->computeTransform(scale, rotation, translation);

		this->transformTo(transform, duration);
	}

	void translateTo(const glm::vec3& targetTranslation, const float& duration = -1.0f)
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, quaternion, translation, skew, perspective);

		glm::mat4 transform = this->computeTransform(scale, glm::mat4_cast(quaternion), targetTranslation);
		this->transformTo(transform, duration);
	}

	void transformTo(const glm::mat4& targetTransform, const float& duration)
	{
		if (duration < 0.0f)
		{
			m_transform = targetTransform;
		}
		else
		{
			m_targetTransform = targetTransform;
			m_targetDuration = duration;
			m_targetDurationTimer = duration;
		}
	}

protected:
	void updateUpdatable(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view)

	{
		if (m_targetDurationTimer > 0.0f)
		{
			m_targetDurationTimer -= elapsedTime;
			m_targetDurationTimer = glm::max(m_targetDurationTimer, 0.0f);

			float delta = (m_targetDuration - m_targetDurationTimer) / m_targetDuration;
			m_transform = glm::interpolate(m_transform, m_targetTransform, delta);
		}

		this->updateMovable(totalTime, elapsedTime, projection, view);
	}

	void initUpdatable(const GLuint& vao, const GLuint& vbo)
	{
		this->initMovable(vao, vbo);
	}

	virtual ~MovableObject() = default;

	virtual void initMovable(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateMovable(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view) = 0;

	glm::mat4 m_transform;

private:
	glm::mat4 computeTransform(const glm::vec3& scale, const glm::mat4& rotation, const glm::vec3& translation)
	{
		glm::mat4 transform = glm::mat4();

		transform *= glm::scale(glm::mat4(), scale); // Multiply by s
		transform *= rotation; // Multiply by R
		transform *= glm::translate(glm::mat4(), translation); // Multiply by t

		return transform;
	}

	glm::mat4 m_targetTransform;
	float m_targetDuration;
	float m_targetDurationTimer;
};

template <class Shader>
class ShapeBase : public MovableObject
{
public:
	ShapeBase(Shader* shader) :
		m_shader(shader)
	{
		if (shader == nullptr)
		{
			std::cout << "NULL shader provided..." << std::endl;
		}
	}

	~ShapeBase()
	{
		delete m_shader;
	}

protected:
	virtual void initShape(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateShape(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view) = 0;

	Shader* m_shader;

private:
	void initMovable(const GLuint& vao, const GLuint& vbo)
	{
		this->initShape(vao, vbo);
	}

	void updateMovable(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view)
	{
		this->updateShape(totalTime, elapsedTime, projection, view);
	}
};

#define NUM_BOX_SIDES 6
template <class Shader>
class BoxBase : public ShapeBase<Shader>
{
static_assert(std::is_base_of<ShaderBase, Shader>::value, "Shader must derive from ShaderBase");

public:
	BoxBase(Shader* shader, glm::vec3 size) :
		ShapeBase<Shader>(shader),
		m_size(size) { }

	BoxBase(Shader* shader) :
		ShapeBase<Shader>(shader)
	{
		m_size = glm::vec3(1.0f);
	}

protected:
	virtual ~BoxBase() = default;

	virtual void initBox(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateBox(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view) = 0;
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
			glm::vec3 n = glm::normalize(glm::cross(a, b));

			// Iterate over vertices
			for (int k = 0; k < 6; k++)
			{
				int m = sequence[k];
				int o = i * 66 + k * 11;

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

	void updateShape(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view)
	{
		this->updateBox(totalTime, elapsedTime, projection, view);
	}

	glm::vec3 m_size;
};

class ColorBox : public BoxBase<ColorShader>
{
public:
	ColorBox(const glm::vec3 color, glm::vec3 size)
		: BoxBase(new ColorShader(), size),
		  m_color(color)
	{
		m_material.shininess = rand() / (float)RAND_MAX;
		m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);// glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
	}

	ColorBox(const Material material, glm::vec3 size)
		: BoxBase(new ColorShader(), size),
		  m_material(material) { }

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

	void updateBox(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view)
	{
		glm::mat4 modelView = view * m_transform;
		glm::mat4 modelViewProj = projection * modelView;
		glm::mat3 modelViewNorm = glm::inverseTranspose(glm::mat3(modelView));

		m_shader->setModelView(modelView);
		m_shader->setModelViewProjection(modelViewProj);
		m_shader->setModelViewNormal(modelViewNorm);

		m_shader->setMaterial(m_material);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	Material m_material;
	glm::vec3 m_color;
};

class UniqueTextureBox : public BoxBase<TextureShader>
{
public:
	UniqueTextureBox(const std::string texturePaths[NUM_BOX_SIDES], glm::vec3 size)
		: BoxBase(new TextureShader(), size)
	{
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
		: BoxBase(new TextureShader(), size)
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

	void updateBox(const float& totalTime, const float& elapsedTime, const glm::mat4& projection, const glm::mat4& view)
	{
		glm::mat4 modelView = view * m_transform;
		glm::mat4 modelViewProj = projection * modelView;
		glm::mat3 modelViewNorm = glm::inverseTranspose(glm::mat3(modelView));

		m_shader->setModelView(modelView);
		m_shader->setModelViewProjection(modelViewProj);
		m_shader->setModelViewNormal(modelViewNorm);

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