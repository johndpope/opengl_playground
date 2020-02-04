#pragma once

#include <glew.h>
#include <glfw3.h>
#include <map>
#include <vector>
#include <camera.h>
#include <shape.h>

template <class T>
class KeyListener
{
public:
	void registerCallback(int key, KeyCallbackFunc func)
	{
		if (m_keyMap.find(key) == m_keyMap.end())
		{
			m_keyMap[key] = std::vector<KeyCallbackFunc>();
		}

		m_keyMap[key].push_back(func);
	}

private:
	typedef void(T::*KeyCallbackFunc)();

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (m_keyMap.find(key) == m_keyMap.end())
		{
			return;
		}

		for (KeyCallbackFunc func : m_keyMap[key])
		{
			func();
		}
	}

	std::map<int, std::vector<KeyCallbackFunc>> m_keyMap;
};

class CameraKeyListener : KeyListener<CameraKeyListener>
{
public:
	CameraKeyListener(Camera& camera, float scale = 0.1f)
		: m_camera(camera),
		  m_scale(scale)
	{
		this->registerCallback(GLFW_KEY_P, plusZ);
		this->registerCallback(GLFW_KEY_M, minusZ);
	}

	void plusZ()
	{
		m_camera.translate(glm::vec3(0, 0, m_scale));
		fprintf(stderr, "<key_callback> P pressed\n");
	}

	void minusZ()
	{
		m_camera.translate(glm::vec3(0, 0, -m_scale));
		fprintf(stderr, "<key_callback> M pressed\n");
	}

private:
	Camera& m_camera;
	float m_scale;
};

template <class T>
class ShapeKeyListener : KeyListener<ShapeKeyListener<T>>
{
public:
	ShapeKeyListener(Shape<T>& shape, float rotate = 45.0f, float scale = 2.0f)
		: m_shape(shape),
		  m_rotate(rotate),
		  m_scale(scale)
	{
		this->registerCallback(GLFW_KEY_B, bigger);
		this->registerCallback(GLFW_KEY_S, smaller);
		this->registerCallback(GLFW_KEY_X, rotateX);
		this->registerCallback(GLFW_KEY_Y, rotateY);
		this->registerCallback(GLFW_KEY_Z, rotateZ);
		this->registerCallback(GLFW_KEY_W, wireframe);
	}

	void bigger()
	{
		m_shape.scale(glm::vec3(m_scale));
		fprintf(stderr, "<key_callback> B pressed");
	}

	void smaller()
	{
		m_shape.scale(glm::vec3(1.0f / m_scale));
		fprintf(stderr, "<key_callback> S pressed");
	}

	void rotateX()
	{
		m_shape.rotate(glm::vec3(1.0f, 0, 0), m_rotate);
	}

	void rotateY()
	{
		m_shape.rotate(glm::vec3(0, 1.0f, 0), m_rotate);
	}

	void rotateZ()
	{
		m_shape.rotate(glm::vec3(0, 0, 1.0f), m_rotate);
	}

	void wireframe()
	{
		m_wireframe = !m_wireframe;
		m_shape.wireframe(m_wireframe);
	}

private:
	Shape<T>& m_shape;
	float m_rotate;
	float m_scale;
	bool m_wireframe;
};