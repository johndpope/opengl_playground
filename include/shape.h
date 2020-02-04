#pragma once
#include <glm.hpp>
#include <glew.h>
#include <string>
#include <movable.h>

template <class Shader>
class ShapeBase : public MovableObject
{
static_assert(std::is_base_of<ShaderBase, Shader>::value, "Shader must derive from ShaderBase");

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