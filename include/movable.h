#pragma once
#include <glm.hpp>
#include <glew.h>
#include <glfw3.h>
#include <glm\gtx\matrix_decompose.hpp>
#include <glm\gtx\matrix_interpolation.hpp>
#include <shader.h>

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

	void update()
	{
		m_totalTime = glfwGetTime() - m_startTime;
		float elapsedTime = m_totalTime - m_lastTotalTime;

		glBindVertexArray(m_vao);
		this->updateUpdatable(m_totalTime, elapsedTime);
		glBindVertexArray(0);

		m_lastTotalTime = m_totalTime;
	}

protected:
	virtual ~UpdatableObject() = default;

	virtual void initUpdatable(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateUpdatable(const float& totalTime, const float& elapsedTime) = 0;

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

	const glm::mat4& pose() const
	{
		return m_pose;
	}

	const glm::vec3 translation() const
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_pose, scale, quaternion, translation, skew, perspective);

		return translation;
	}

	const glm::mat4 rotation() const
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_pose, scale, quaternion, translation, skew, perspective);

		return glm::mat4_cast(quaternion);
	}

	void scale(const glm::vec3& scale)
	{
		m_pose = glm::scale(m_pose, scale);
	}

	void rotate(const glm::vec3& axis, const float& angleInDeg)
	{
		m_pose = glm::rotate(m_pose, (float)glm::radians(angleInDeg), glm::normalize(axis));
	}

	void translate(const glm::vec3& translation)
	{
		m_pose = glm::translate(m_pose, translation);
	}

	void scaleTo(const glm::vec3& targetScale, const float& duration = -1.0f)
	{
		glm::vec3 scale;
		glm::quat quaternion;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_pose, scale, quaternion, translation, skew, perspective);

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
		glm::decompose(m_pose, scale, quaternion, translation, skew, perspective);

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
		glm::decompose(m_pose, scale, quaternion, translation, skew, perspective);

		glm::mat4 transform = this->computeTransform(scale, glm::mat4_cast(quaternion), targetTranslation);
		this->transformTo(transform, duration);
	}

	void transformTo(const glm::mat4& targetTransform, const float& duration = -1.0f)
	{
		if (duration < 0.0f)
		{
			m_pose = targetTransform;
		}
		else
		{
			m_startTransform = m_pose;
			m_targetTransform = targetTransform;
			m_targetDuration = duration;
			m_targetDurationTimer = duration;
		}
	}

protected:
	void updateUpdatable(const float& totalTime, const float& elapsedTime)
	{
		if (m_targetDurationTimer > 0.0f)
		{
			m_targetDurationTimer -= elapsedTime;
			m_targetDurationTimer = glm::max(m_targetDurationTimer, 0.0f);

			float delta = (m_targetDuration - m_targetDurationTimer) / m_targetDuration;
			m_pose = glm::interpolate(m_startTransform, m_targetTransform, delta);
		}

		this->updateMovable(totalTime, elapsedTime);
	}

	void initUpdatable(const GLuint& vao, const GLuint& vbo)
	{
		this->initMovable(vao, vbo);
	}

	virtual ~MovableObject() = default;

	virtual void initMovable(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateMovable(const float& totalTime, const float& elapsedTime) = 0;

private:
	glm::mat4 computeTransform(const glm::vec3& scale, const glm::mat4& rotation, const glm::vec3& translation)
	{
		glm::mat4 transform = glm::mat4();

		transform *= glm::scale(glm::mat4(), scale); // Multiply by s
		transform *= rotation; // Multiply by R
		transform *= glm::translate(glm::mat4(), translation); // Multiply by t

		return transform;
	}

	glm::mat4 m_pose;
	glm::mat4 m_startTransform;
	glm::mat4 m_targetTransform;
	float m_targetDuration;
	float m_targetDurationTimer;
};