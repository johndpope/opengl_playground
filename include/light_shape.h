#pragma once
#include <box.h>
#include <light.h>

class LightBox : public Light
{
public:
	LightBox(const glm::vec4& color, const glm::vec3 size = glm::vec3(0.05f));
	LightBox(const glm::vec3 size = glm::vec3(0.05f));
	~LightBox();

	void update(const Camera& camera);

private:
	using UpdatableObject::update;

	void initLight(const GLuint& vao, const GLuint& vbo);
	void updateLight(const float& totalTime, const float& frameTime);

	BasicColorBox* m_box;
};