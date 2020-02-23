#pragma once
#include <glm.hpp>
#include <glew.h>
#include <camera.h>
#include <material.h>
#include <shader.h>
#include <shape.h>
#include <vector>

#define NUM_BOX_SIDES 6
#define VERTICES_PER_SIDE 6

struct BoxVertexAttribute
{
    glm::vec3 position;
    glm::vec2 texture;
    glm::vec3 normal;
    glm::vec4 color;
};

class Box : public Shape
{
public:
	Box(ShaderBase* shader, const glm::vec3& size) :
		Shape(shader),
		m_size(size) { }

	Box(ShaderBase* shader) : Box(shader, glm::vec3(1.0f)) { }

protected:
	virtual ~Box() = default;

	virtual void initBox(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light) = 0;
	virtual void generateCustomBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size);

	void generateBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size, glm::vec4* color = nullptr);

	int m_numVertices;

private:
	void initShape(const GLuint& vao, const GLuint& vbo);
	void updateShape(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

    glm::vec3 m_size;
};

class BasicColorBox : public Box
{
public:
	BasicColorBox(const glm::vec4 color, const glm::vec3& size);
	BasicColorBox(const glm::vec3& size);

private:
	void generateCustomBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size);
	void initBox(const GLuint& vao, const GLuint& vbo);
	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

	glm::vec4 m_color;
};

class ColorBox : public Box
{
public:
	ColorBox(const glm::vec4 color, glm::vec3 size);
	ColorBox(const Material material, glm::vec3 size);

private:
	void generateCustomBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size);
	void initBox(const GLuint& vao, const GLuint& vbo);
	void updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

	Material m_material;
	glm::vec4 m_color;
};

class MultiTextureBox : public Box
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