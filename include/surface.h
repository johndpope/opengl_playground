#pragma once
#include <glm.hpp>
#include <string>
#include <movable.h>
#include <light.h>
#include <camera.h>

class Surface : public MovableObject
{
public:
	Surface(ShaderBase* shader);
	~Surface();

	void wireframe(bool enable);
	void update(const Camera& camera, const Light& light);

protected:
	virtual void initSurface(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light) = 0;

	ShaderBase* m_shader;
	GLenum m_wireframe;

private:
	using UpdatableObject::update;

	void initMovable(const GLuint& vao, const GLuint& vbo);
	void updateMovable(const float& totalTime, const float& frameTime);

	const Camera* m_camera;
	const Light* m_light;
};