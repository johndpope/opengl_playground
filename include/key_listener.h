#pragma once

#include <glew.h>
#include <glfw3.h>
#include <map>
#include <vector>
#include <camera.h>
#include <shape.h>
#include <functional>

class KeyListener
{
protected:
	typedef std::function<void()> KeyCallbackFunc;

	static void registerCallback(GLFWwindow* window, int key, KeyCallbackFunc func)
	{
		if (registered(window, key, func))
		{
			return;
		}

		if (m_windowKeyMap.find(window) == m_windowKeyMap.end())
		{
			glfwSetKeyCallback(window, &KeyListener::keyCallback);

			m_windowKeyMap[window] = std::map<int, std::vector<KeyCallbackFunc>>();
		}

		if (m_windowKeyMap[window].find(key) == m_windowKeyMap[window].end())
		{
			m_windowKeyMap[window][key] = std::vector<KeyCallbackFunc>();
		}

		m_windowKeyMap[window][key].push_back(func);
	}

	template <class Info>
	static void callback(std::function<void(Info&)> func, std::vector<Info> instances)
	{
		for (Info& instance : instances)
		{
			func(instance);
		}
	}

	static bool registered(GLFWwindow* window, int key, KeyCallbackFunc func)
	{
		if (m_windowKeyMap.find(window) == m_windowKeyMap.end())
		{
			return false;
		}

		if (m_windowKeyMap[window].find(key) == m_windowKeyMap[window].end())
		{
			return false;;
		}

		for (KeyCallbackFunc& currentFunc : m_windowKeyMap[window][key])
		{
			if (getAddress(currentFunc) == getAddress(func))
			{
				return true;
			}
		}

		return false;
	}

private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action != GLFW_PRESS)
		{
			return;
		}

		if (m_windowKeyMap[window].find(key) == m_windowKeyMap[window].end())
		{
			return;
		}

		for (KeyCallbackFunc func : m_windowKeyMap[window][key])
		{
			func();
		}
	}

	static size_t getAddress(KeyCallbackFunc f)
	{
		typedef void (fnType)();
		fnType ** fnPointer = f.template target<fnType*>();
		return (size_t)*fnPointer;
	}

	static std::map<GLFWwindow*, std::map<int, std::vector<KeyCallbackFunc>>> m_windowKeyMap;
};

class CameraKeyListener : private KeyListener
{
public:
	CameraKeyListener(GLFWwindow* window, Camera& camera, float scale = 0.1f)
		: m_camera(camera),
		  m_scale(scale)
	{
		registerCallback(window, GLFW_KEY_P, KeyCallbackFunc(std::bind(&CameraKeyListener::plusZ, this)));
		registerCallback(window, GLFW_KEY_M, KeyCallbackFunc(std::bind(&CameraKeyListener::minusZ, this)));
	}

	~CameraKeyListener() = default;

private:
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

	Camera& m_camera;
	float m_scale;
};

template <class T>
class ShapeKeyListener : KeyListener
{
public:
	ShapeKeyListener(GLFWwindow* window, Shape<T>& shape, float rotate = 45.0f, float scale = 2.0f)
		: m_shape(shape),
		  m_rotate(rotate),
		  m_scale(scale)
	{
		registerCallback(window, GLFW_KEY_B, KeyCallbackFunc(std::bind(&ShapeKeyListener::bigger, this)));
		registerCallback(window, GLFW_KEY_S, KeyCallbackFunc(std::bind(&ShapeKeyListener::smaller, this)));
		registerCallback(window, GLFW_KEY_X, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateX, this)));
		registerCallback(window, GLFW_KEY_Y, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Z, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateZ, this)));
		registerCallback(window, GLFW_KEY_W, KeyCallbackFunc(std::bind(&ShapeKeyListener::wireframe, this)));
	}

	~ShapeKeyListener() = default;

private:
	void bigger()
	{
		m_shape.scale(glm::vec3(m_scale));
		fprintf(stderr, "<key_callback> B pressed\n");
	}

	void smaller()
	{
		m_shape.scale(glm::vec3(1.0f / m_scale));
		fprintf(stderr, "<key_callback> S pressed\n");
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

	Shape<T>& m_shape;
	float m_rotate;
	float m_scale;
	bool m_wireframe;
};