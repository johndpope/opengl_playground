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

	static void registerCallback(GLFWwindow* window, int key, int action, KeyCallbackFunc func)
	{
		if (m_windowKeyMap.find(window) == m_windowKeyMap.end())
		{
			glfwSetKeyCallback(window, &KeyListener::keyCallback);

			m_windowKeyMap[window] = std::map<int, std::vector<KeyCallbackFuncArgs>>();
		}

		if (m_windowKeyMap[window].find(key) == m_windowKeyMap[window].end())
		{
			m_windowKeyMap[window][key] = std::vector<KeyCallbackFuncArgs>();
		}

		KeyCallbackFuncArgs args;
		args.func = func;
		args.action = action;
		m_windowKeyMap[window][key].push_back(args);
	}

private:
	struct KeyCallbackFuncArgs
	{
		KeyCallbackFunc func;
		int action;
	};

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (m_windowKeyMap[window].find(key) == m_windowKeyMap[window].end())
		{
			return;
		}

		for (KeyCallbackFuncArgs args : m_windowKeyMap[window][key])
		{
			if (args.action == action)
			{
				args.func();
			}
		}
	}

	static std::map<GLFWwindow*, std::map<int, std::vector<KeyCallbackFuncArgs>>> m_windowKeyMap;
};

class CameraKeyListener : private KeyListener
{
public:
	CameraKeyListener(GLFWwindow* window, Camera& camera, float rotateSpeed = 1.0f, float translateSpeed = 0.2f, float scale = 0.1f)
		: m_camera(camera),
		  m_scale(scale),
		  m_rotateSpeed(rotateSpeed),
		  m_translateSpeed(translateSpeed)
	{
		registerCallback(window, GLFW_KEY_A, GLFW_PRESS, KeyCallbackFunc(std::bind(&CameraKeyListener::walkLeft, this)));
		registerCallback(window, GLFW_KEY_A, GLFW_REPEAT, KeyCallbackFunc(std::bind(&CameraKeyListener::walkLeft, this)));
		registerCallback(window, GLFW_KEY_A, GLFW_RELEASE, KeyCallbackFunc(std::bind(&CameraKeyListener::walkLeft, this)));
		registerCallback(window, GLFW_KEY_W, GLFW_PRESS, KeyCallbackFunc(std::bind(&CameraKeyListener::walkForward, this)));
		registerCallback(window, GLFW_KEY_W, GLFW_REPEAT, KeyCallbackFunc(std::bind(&CameraKeyListener::walkForward, this)));
		registerCallback(window, GLFW_KEY_W, GLFW_RELEASE, KeyCallbackFunc(std::bind(&CameraKeyListener::walkForward, this)));
		registerCallback(window, GLFW_KEY_S, GLFW_PRESS, KeyCallbackFunc(std::bind(&CameraKeyListener::walkBackward, this)));
		registerCallback(window, GLFW_KEY_S, GLFW_REPEAT, KeyCallbackFunc(std::bind(&CameraKeyListener::walkBackward, this)));
		registerCallback(window, GLFW_KEY_S, GLFW_RELEASE, KeyCallbackFunc(std::bind(&CameraKeyListener::walkBackward, this)));
		registerCallback(window, GLFW_KEY_D, GLFW_PRESS, KeyCallbackFunc(std::bind(&CameraKeyListener::walkRight, this)));
		registerCallback(window, GLFW_KEY_D, GLFW_REPEAT, KeyCallbackFunc(std::bind(&CameraKeyListener::walkRight, this)));
		registerCallback(window, GLFW_KEY_D, GLFW_RELEASE, KeyCallbackFunc(std::bind(&CameraKeyListener::walkRight, this)));
		registerCallback(window, GLFW_KEY_J, GLFW_PRESS, KeyCallbackFunc(std::bind(&CameraKeyListener::rotateLeft, this)));
		registerCallback(window, GLFW_KEY_J, GLFW_REPEAT, KeyCallbackFunc(std::bind(&CameraKeyListener::rotateLeft, this)));
		registerCallback(window, GLFW_KEY_J, GLFW_RELEASE, KeyCallbackFunc(std::bind(&CameraKeyListener::rotateLeft, this)));
		registerCallback(window, GLFW_KEY_L, GLFW_PRESS, KeyCallbackFunc(std::bind(&CameraKeyListener::rotateRight, this)));
		registerCallback(window, GLFW_KEY_L, GLFW_REPEAT, KeyCallbackFunc(std::bind(&CameraKeyListener::rotateRight, this)));
		registerCallback(window, GLFW_KEY_L, GLFW_RELEASE, KeyCallbackFunc(std::bind(&CameraKeyListener::rotateRight, this)));
	}

	~CameraKeyListener() = default;

private:
	void rotateLeft()
	{
		rotate(-m_rotateSpeed);
	}

	void rotateRight()
	{
		rotate(m_rotateSpeed);
	}

	void walkLeft()
	{
		translate(glm::vec3(1.0f, 0, 0), m_translateSpeed);
	}

	void walkRight()
	{
		translate(glm::vec3(1.0f, 0, 0), -m_translateSpeed);
	}

	void walkForward()
	{
		translate(glm::vec3(0, 0, 1.0f), m_translateSpeed);
	}

	void walkBackward()
	{
		translate(glm::vec3(0, 0, 1.0f), -m_translateSpeed);
	}

	void rotate(float angle)
	{
		glm::vec3 translation = m_camera.translation();
		m_camera.translate(-translation);
		m_camera.rotate(glm::vec3(0, 1.0f, 0), angle);
		m_camera.translate(translation);
	}

	void translate(glm::vec3 moveDirection, float speed)
	{
		glm::vec3 translation = glm::normalize(glm::vec3(m_camera.rotation() * glm::vec4(moveDirection, 1.0f)));
		translation *= speed;
		m_camera.translate(translation);
	}

	Camera& m_camera;
	float m_rotateSpeed;
	float m_translateSpeed;
	float m_scale;
};

template <class S>
class ShapeKeyListener : KeyListener
{
public:
	ShapeKeyListener(GLFWwindow* window, S& shape, float rotate = 45.0f, float scale = 2.0f)
		: m_shape(shape),
		  m_rotate(rotate),
		  m_scale(scale)
	{
		registerCallback(window, GLFW_KEY_EQUAL, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::bigger, this)));
		registerCallback(window, GLFW_KEY_MINUS, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::smaller, this)));
		registerCallback(window, GLFW_KEY_X, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateX, this)));
		registerCallback(window, GLFW_KEY_X, GLFW_REPEAT, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateX, this)));
		registerCallback(window, GLFW_KEY_X, GLFW_RELEASE, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateX, this)));
		registerCallback(window, GLFW_KEY_Y, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Y, GLFW_REPEAT, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Y, GLFW_RELEASE, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Z, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateZ, this)));
		registerCallback(window, GLFW_KEY_Z, GLFW_REPEAT, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateZ, this)));
		registerCallback(window, GLFW_KEY_Z, GLFW_RELEASE, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateZ, this)));
		registerCallback(window, GLFW_KEY_F, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::wireframe, this)));
	}

	~ShapeKeyListener() = default;

private:
	void bigger()
	{
		m_shape.scale(glm::vec3(m_scale));
		fprintf(stdout, "<key_callback> + pressed\n");
	}

	void smaller()
	{
		m_shape.scale(glm::vec3(1.0f / m_scale));
		fprintf(stdout, "<key_callback> - pressed\n");
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

	S& m_shape;
	float m_rotate;
	float m_scale;
	bool m_wireframe;
};