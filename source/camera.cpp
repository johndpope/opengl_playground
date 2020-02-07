#pragma once
#include <camera.h>

void Camera::lookAt(glm::vec3 point, float duration)
{
	glm::vec3 translation = this->translation();
	glm::mat4 transform = glm::lookAt(translation, point, glm::vec3(0.0f, 1.0f, 0.0f));

	this->transformTo(transform, duration);
}

OrthographicCamera::OrthographicCamera(
	float left,
	float right,
	float bottom,
	float top,
	float verticalFov,
	float nearClipPlane,
	float farClipPlane)
{
	m_projection = glm::ortho(left, right, bottom, top, nearClipPlane, farClipPlane);
}

const glm::mat4& OrthographicCamera::projection() const
{
	return m_projection;
}

PerspectiveCamera::PerspectiveCamera(
	unsigned int width,
	unsigned int height,
	float verticalFov,
	float nearClipPlane,
	float farClipPlane)
{
	m_projection = glm::perspective(glm::radians(verticalFov), width / (float)height, nearClipPlane, farClipPlane);
}

const glm::mat4& PerspectiveCamera::projection() const
{
	return m_projection;
}