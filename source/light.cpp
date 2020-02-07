#pragma once
#include <light.h>

void Light::initMovable(const GLuint& vao, const GLuint& vbo)
{
	this->initLight(vao, vbo);
}

void Light::updateMovable(const float& totalTime, const float& frameTime)
{
	this->updateLight(totalTime, frameTime);
}