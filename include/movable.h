#pragma once
#include <glm.hpp>
#include <glew.h>
#include <glfw3.h>
#include <glm\gtx\matrix_decompose.hpp>
#include <glm\gtx\matrix_interpolation.hpp>
#include <functional>
#include <stack>
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
		float frameTime = m_totalTime - m_lastTotalTime;

		glBindVertexArray(m_vao);
		this->updateUpdatable(m_totalTime, frameTime);
		glBindVertexArray(0);

		m_lastTotalTime = m_totalTime;
	}

protected:
	virtual ~UpdatableObject() = default;

	virtual void initUpdatable(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateUpdatable(const float& totalTime, const float& frameTime) = 0;

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
		OneShotMovementData* data = new OneShotMovementData();

		data->update = UpdateMovement(std::bind(&MovableObject::computeOneShotMovement, this, std::placeholders::_1));
		data->duration = duration;
		data->startTransform = m_pose;
		data->targetTransform = targetTransform;

		m_moveDatas.push(data);
	}

	void orbit(const glm::vec3& targetAxis, const glm::vec3& center, const float& radius, const float& speed, const float& duration = -1.0f)
	{
		OrbitMovementData* data = new OrbitMovementData();

		data->update = UpdateMovement(std::bind(&MovableObject::computeOrbitMovement, this, std::placeholders::_1));
		data->duration = duration < 0.0 ? (float)((unsigned int)-1) : duration;
		data->center = center;
		data->radius = radius;
		data->speed = speed;
		data->targetAxis = targetAxis;

		m_moveDatas.push(data);

		this->transformTo(this->computeOrbitMovement(data), 0.5f);
	}

protected:
	void updateUpdatable(const float& totalTime, const float& frameTime)
	{
		bool keepUpdating = true;

		while (!m_moveDatas.empty() && keepUpdating)
		{
			void* moveData = m_moveDatas.top();

			MovementData* data = static_cast<MovementData*>(moveData);
			data->totalTime = totalTime;
			data->frameTime = frameTime;
			data->elapsedTime += frameTime;

			m_pose = data->update(data);

			if (data->elapsedTime > data->duration)
			{
				m_moveDatas.pop();
				delete moveData;
			}
			else
			{
				keepUpdating = false;
			}
		}

		this->updateMovable(totalTime, frameTime);
	}

	void initUpdatable(const GLuint& vao, const GLuint& vbo)
	{
		this->initMovable(vao, vbo);
	}

	virtual ~MovableObject() = default;

	virtual void initMovable(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateMovable(const float& totalTime, const float& frameTime) = 0;

private:
	typedef std::function<glm::mat4(void*)> UpdateMovement;

	struct MovementData
	{
		UpdateMovement update;
		float totalTime;
		float frameTime;
		float duration;
		float elapsedTime = 0.0f;
	};

	struct OneShotMovementData : MovementData
	{
		glm::mat4 startTransform;
		glm::mat4 targetTransform;
	};

	struct OrbitMovementData : MovementData
	{
		glm::vec3 targetAxis;
		glm::vec3 center;
		float radius;
		float speed;
	};

	glm::mat4 computeOneShotMovement(void* movementData)
	{
		OneShotMovementData* data = static_cast<OneShotMovementData*>(movementData);

		float delta = (data->duration <= 0.0f) ? 1.0f : data->elapsedTime / data->duration;
		return glm::interpolate(data->startTransform, data->targetTransform, delta);
	}

	glm::mat4 computeOrbitMovement(void* movementData)
	{
		OrbitMovementData* data = static_cast<OrbitMovementData*>(movementData);

		// Require 2 perpendicular vectors to rotational axis
		glm::vec3 n = glm::normalize(data->targetAxis);
		glm::vec3 a;
		if (n.x != 0.0f)
		{
			a = glm::normalize(glm::vec3(-1 * (n.z + n.y) / n.x, 1.0f, 1.0f));
		}
		else if (n.y != 0.0f)
		{
			a = glm::normalize(glm::vec3(1.0f, -1 * (n.x + n.z) / n.y, 1.0f));
		}
		else if (n.z != 0.0f)
		{
			a = glm::normalize(glm::vec3(1.0f, 1.0f, -1 * (n.x + n.y) / n.z));
		}
		else
		{
			fprintf(stderr, "TargetAxis is 0 vector, cannot compute orbit\n");
		}

		glm::vec3 b = glm::normalize(glm::cross(a, n));

		float theta = data->speed * data->elapsedTime;
		float x = data->radius * (glm::cos(theta) * a.x + glm::sin(theta) * b.x);
		float y = data->radius * (glm::cos(theta) * a.y + glm::sin(theta) * b.y);
		float z = data->radius * (glm::cos(theta) * a.z + glm::sin(theta) * b.z);

		glm::vec3 position = glm::vec3(x, y, z) + data->center;

		data->targetAxis = glm::rotate(glm::mat4(), (float)glm::radians(data->speed * 0.01f), a) * glm::vec4(n, 1.0f);

		return glm::translate(glm::mat4(), position);
	}

	glm::mat4 computeTransform(const glm::vec3& scale, const glm::mat4& rotation, const glm::vec3& translation)
	{
		glm::mat4 transform = glm::mat4();

		transform *= glm::scale(glm::mat4(), scale); // Multiply by s
		transform *= rotation; // Multiply by R
		transform *= glm::translate(glm::mat4(), translation); // Multiply by t

		return transform;
	}

	glm::mat4 m_pose;
	std::stack<void*> m_moveDatas;
};