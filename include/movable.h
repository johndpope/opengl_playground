#pragma once
#include <glm.hpp>
#include <glew.h>
#include <functional>
#include <stack>
#include <shader.h>

class UpdatableObject
{
public:
	UpdatableObject() { };

	void init();
	void update();

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

	const glm::mat4& pose() const;
	const glm::vec3 translation() const;
	const glm::mat4 rotation() const;

	void scale(const glm::vec3& scale);

	void rotate(const glm::vec3& axis, const float& angleInDeg);
	void translate(const glm::vec3& translation);
	void scaleTo(const glm::vec3& targetScale, const float& duration = -1.0f);
	void rotateTo(const glm::vec3& targetAxis, const float& targetAngleInDeg, const float& duration = -1.0f);
	void translateTo(const glm::vec3& targetTranslation, const float& duration = -1.0f);
	void transformTo(const glm::mat4& targetTransform, const float& duration = -1.0f);
	void orbit(const glm::vec3& targetAxis, const glm::vec3& center, const float& radius, const float& circleSpeed, const float& sphereSpeed, const float& duration = -1.0f);

protected:
	void updateUpdatable(const float& totalTime, const float& frameTime);
	void initUpdatable(const GLuint& vao, const GLuint& vbo);

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
		float circleSpeed;
		float sphereSpeed;
	};

	glm::mat4 computeOneShotMovement(void* movementData);
	glm::mat4 computeOrbitMovement(void* movementData);
	glm::mat4 computeTransform(const glm::vec3& scale, const glm::mat4& rotation, const glm::vec3& translation);

	glm::mat4 m_pose;
	std::stack<void*> m_moveDatas;
};