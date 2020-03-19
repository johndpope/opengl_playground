#pragma once
#include <light_shape.h>

LightBox::LightBox(const glm::vec4& color)
	: m_box(new BasicColorBox(color, glm::vec3(5.0f))) { }

LightBox::LightBox()
	: m_box(new BasicColorBox(glm::vec4(1.0f), glm::vec3(5.0f))) { }

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