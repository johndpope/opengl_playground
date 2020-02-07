#pragma once
#include <movable.h>
#include <glm.hpp>
#include <glew.h>
#include <glm\gtc\matrix_transform.hpp>

class Camera : public MovableObject
{
public:
	virtual const glm::mat4& projection() const = 0;
	void lookAt(glm::vec3 point, float duration = -1.0f);

private:
	void initMovable(const GLuint& vao, const GLuint& vbo) { }
	void updateMovable(const float& totalTime, const float& frameTime) { }
};

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(
		float left,
		float right,
		float bottom,
		float top,
		float verticalFov,
		float nearClipPlane,
		float farClipPlane);

	const glm::mat4& projection() const;

private:
	glm::mat4 m_projection;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(
		unsigned int width,
		unsigned int height,
		float verticalFov,
		float nearClipPlane,
		float farClipPlane);

	const glm::mat4& projection() const;

private:
	glm::mat4 m_projection;
};