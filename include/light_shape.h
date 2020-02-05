#pragma once
#include <glm.hpp>
#include <glew.h>
#include <box.h>
#include <light.h>

class LightBox : public Light
{
public:
	LightBox(const glm::vec3& color)
		: m_box(new BasicColorBox(color, glm::vec3(0.1f))) { }

	LightBox()
		: m_box(new BasicColorBox(glm::vec3(1.0f), glm::vec3(0.1f))) { }

	~LightBox()
	{
		delete m_box;
	}

	void update(const Camera& camera)
	{
		UpdatableObject::update();
		m_box->update(camera, *this);
	}

private:
	using UpdatableObject::update;

	void initLight(const GLuint& vao, const GLuint& vbo)
	{
		m_box->init();
	}

	void updateLight(const float& totalTime, const float& elapsedTime)
	{
		m_box->transformTo(m_transform);
	}

	BasicColorBox* m_box;
};