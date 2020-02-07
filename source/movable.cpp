#pragma once
#include <movable.h>

#include <glfw3.h>
#include <glm\gtx\matrix_decompose.hpp>
#include <glm\gtx\matrix_interpolation.hpp>

void UpdatableObject::init()
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

void UpdatableObject::update()
{
	m_totalTime = glfwGetTime() - m_startTime;
	float frameTime = m_totalTime - m_lastTotalTime;

	glBindVertexArray(m_vao);
	this->updateUpdatable(m_totalTime, frameTime);
	glBindVertexArray(0);

	m_lastTotalTime = m_totalTime;
}

const glm::mat4& MovableObject::pose() const
{
	return m_pose;
}

const glm::vec3 MovableObject::translation() const
{
	glm::vec3 scale;
	glm::quat quaternion;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m_pose, scale, quaternion, translation, skew, perspective);

	return translation;
}

const glm::mat4 MovableObject::rotation() const
{
	glm::vec3 scale;
	glm::quat quaternion;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m_pose, scale, quaternion, translation, skew, perspective);

	return glm::mat4_cast(quaternion);
}

void MovableObject::scale(const glm::vec3& scale)
{
	m_pose = glm::scale(m_pose, scale);
}

void MovableObject::rotate(const glm::vec3& axis, const float& angleInDeg)
{
	m_pose = glm::rotate(m_pose, (float)glm::radians(angleInDeg), glm::normalize(axis));
}

void MovableObject::translate(const glm::vec3& translation)
{
	m_pose = glm::translate(m_pose, translation);
}

void MovableObject::scaleTo(const glm::vec3& targetScale, const float& duration)
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

void MovableObject::rotateTo(const glm::vec3& targetAxis, const float& targetAngleInDeg, const float& duration)
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

void MovableObject::translateTo(const glm::vec3& targetTranslation, const float& duration)
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

void MovableObject::transformTo(const glm::mat4& targetTransform, const float& duration)
{
	OneShotMovementData* data = new OneShotMovementData();

	data->update = UpdateMovement(std::bind(&MovableObject::computeOneShotMovement, this, std::placeholders::_1));
	data->duration = duration;
	data->startTransform = m_pose;
	data->targetTransform = targetTransform;

	m_moveDatas.push(data);
}

void MovableObject::orbit(const glm::vec3& targetAxis, const glm::vec3& center, const float& radius, const float& speed, const float& duration)
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

void MovableObject::updateUpdatable(const float& totalTime, const float& frameTime)
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

void MovableObject::initUpdatable(const GLuint& vao, const GLuint& vbo)
{
	this->initMovable(vao, vbo);
}

glm::mat4 MovableObject::computeOneShotMovement(void* movementData)
{
	OneShotMovementData* data = static_cast<OneShotMovementData*>(movementData);

	float delta = (data->duration <= 0.0f) ? 1.0f : data->elapsedTime / data->duration;
	return glm::interpolate(data->startTransform, data->targetTransform, delta);
}

glm::mat4 MovableObject::computeOrbitMovement(void* movementData)
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

glm::mat4 MovableObject::computeTransform(const glm::vec3& scale, const glm::mat4& rotation, const glm::vec3& translation)
{
	glm::mat4 transform = glm::mat4();

	transform *= glm::scale(glm::mat4(), scale); // Multiply by s
	transform *= rotation; // Multiply by R
	transform *= glm::translate(glm::mat4(), translation); // Multiply by t

	return transform;
}