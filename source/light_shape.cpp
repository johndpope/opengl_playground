#pragma once
#include <light_shape.h>

LightBox::LightBox(const glm::vec4& color, const glm::vec3 size)
	: m_box(new BasicColorBox(color, size)) { }

LightBox::LightBox(const glm::vec3 size)
	: m_box(new BasicColorBox(glm::vec4(1.0f), size)) { }

LightBox::~LightBox()
{
	delete m_box;
}

void LightBox::update(const Camera& camera)
{
	UpdatableObject::update();
	m_box->update(camera, *this);
}

void LightBox::initLight(const GLuint& vao, const GLuint& vbo)
{
	m_box->init();
}

void LightBox::updateLight(const float& totalTime, const float& frameTime)
{
	m_box->transformTo(this->pose());
}