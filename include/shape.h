#pragma once
#include <glm.hpp>
#include <glew.h>
#include <string>
#include <movable.h>
#include <light.h>
#include <camera.h>

template <class Shader>
class Shape : public MovableObject
{
static_assert(std::is_base_of<ShaderBase, Shader>::value, "Shader must derive from ShaderBase");

public:
	Shape(Shader* shader) :
		m_shader(shader)
	{
		if (shader == nullptr)
		{
			std::cout << "NULL shader provided..." << std::endl;
		}
	}

	void wireframe(bool enable)
	{
		m_wireframe = enable ? GL_LINE : GL_FILL;
	}

	~Shape()
	{
		delete m_shader;
	}

	void update(const Camera& camera, const Light& light)
	{
		m_camera = &camera;
		m_light = &light;
		UpdatableObject::update();
	}

protected:
	virtual void initShape(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateShape(const float& totalTime, const float& elapsedTime, const Camera& camera, const Light& light) = 0;

	Shader* m_shader;
	GLenum m_wireframe;

private:
	using UpdatableObject::update;

	void initMovable(const GLuint& vao, const GLuint& vbo)
	{
		m_wireframe = GL_FILL;

		this->initShape(vao, vbo);
	}

	void updateMovable(const float& totalTime, const float& elapsedTime) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, m_wireframe);

		this->updateShape(totalTime, elapsedTime, *m_camera, *m_light);
	}

	const Camera* m_camera;
	const Light* m_light;
};