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

	static void registerCallback(GLFWwindow* window, int key, int action, KeyCallbackFunc func);

private:
	struct KeyCallbackFuncArgs
	{
		KeyCallbackFunc func;
		int action;
	};

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static std::map<GLFWwindow*, std::map<int, std::vector<KeyCallbackFuncArgs>>> m_windowKeyMap;
};

class CameraKeyListener : private KeyListener
{
public:
	CameraKeyListener(GLFWwindow* window, Camera& camera, float rotateSpeed = 1.0f, float translateSpeed = 0.2f, float scale = 0.1f);
	~CameraKeyListener() = default;

private:
	void rotateLeft();
	void rotateRight();
	void walkLeft();
	void walkRight();
	void walkForward();
	void walkBackward();
	void rotate(float angle);
	void translate(glm::vec3 moveDirection, float speed);

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
		registerCallback(window, GLFW_KEY_Y, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Y, GLFW_REPEAT, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Z, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateZ, this)));
		registerCallback(window, GLFW_KEY_Z, GLFW_REPEAT, KeyCallbackFunc(std::bind(&ShapeKeyListener::rotateZ, this)));
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