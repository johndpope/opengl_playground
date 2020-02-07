#pragma once
#include <glm.hpp>
#include <glew.h>
#include <movable.h>

class Light : public MovableObject
{
protected:
	virtual void initLight(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateLight(const float& totalTime, const float& frameTime) = 0;

private:
	void initMovable(const GLuint& vao, const GLuint& vbo)
	{
		this->initLight(vao, vbo);
	}

	void updateMovable(const float& totalTime, const float& frameTime)
	{
		this->updateLight(totalTime, frameTime);
	}
};

class MockLight : public Light
{
private:
	void initLight(const GLuint& vao, const GLuint& vbo) { };
	void updateLight(const float& totalTime, const float& frameTime) { };
};