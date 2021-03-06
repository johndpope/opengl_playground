#pragma once
#include <glm.hpp>
#include <glew.h>
#include <string>
#include <movable.h>
#include <light.h>
#include <camera.h>

class Shape : public MovableObject
{
public:
	Shape(ShaderBase* shader) :
		m_shader(shader)
	{
		if (shader == nullptr)
		{
			fprintf(stdout, "NULL shader provided...\n");
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
	virtual void updateShape(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light) = 0;

	ShaderBase* m_shader;
	GLenum m_wireframe;

private:
	using UpdatableObject::update;

	void initMovable(const GLuint& vao, const GLuint& vbo)
	{
		m_wireframe = GL_FILL;

		m_shader->use();
		this->initShape(vao, vbo);
	}

	void updateMovable(const float& totalTime, const float& frameTime)
	{
		glPolygonMode(GL_FRONT_AND_BACK, m_wireframe);

		m_shader->use();
		this->updateShape(totalTime, frameTime, *m_camera, *m_light);
	}

	const Camera* m_camera;
	const Light* m_light;
};