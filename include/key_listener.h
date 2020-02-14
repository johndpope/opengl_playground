#pragma once

#include <glew.h>
#include <glfw3.h>
#include <map>
#include <vector>
#include <camera.h>
#include <shape.h>
#include <functional>
#include <movable.h>

class KeyListener
{
public:
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

class MovableKeyListener : public KeyListener
{
public:
	MovableKeyListener(GLFWwindow* window, MovableObject& movable, float rotate = 1.0f, float scale = 1.01f)
		: m_movable(movable),
		  m_rotate(rotate),
		  m_scale(scale)
	{
		registerCallback(window, GLFW_KEY_EQUAL, GLFW_PRESS, KeyCallbackFunc(std::bind(&MovableKeyListener::bigger, this)));
		registerCallback(window, GLFW_KEY_EQUAL, GLFW_REPEAT, KeyCallbackFunc(std::bind(&MovableKeyListener::bigger, this)));
		registerCallback(window, GLFW_KEY_MINUS, GLFW_PRESS, KeyCallbackFunc(std::bind(&MovableKeyListener::smaller, this)));
		registerCallback(window, GLFW_KEY_MINUS, GLFW_REPEAT, KeyCallbackFunc(std::bind(&MovableKeyListener::smaller, this)));
		registerCallback(window, GLFW_KEY_X, GLFW_PRESS, KeyCallbackFunc(std::bind(&MovableKeyListener::rotateX, this)));
		registerCallback(window, GLFW_KEY_X, GLFW_REPEAT, KeyCallbackFunc(std::bind(&MovableKeyListener::rotateX, this)));
		registerCallback(window, GLFW_KEY_Y, GLFW_PRESS, KeyCallbackFunc(std::bind(&MovableKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Y, GLFW_REPEAT, KeyCallbackFunc(std::bind(&MovableKeyListener::rotateY, this)));
		registerCallback(window, GLFW_KEY_Z, GLFW_PRESS, KeyCallbackFunc(std::bind(&MovableKeyListener::rotateZ, this)));
		registerCallback(window, GLFW_KEY_Z, GLFW_REPEAT, KeyCallbackFunc(std::bind(&MovableKeyListener::rotateZ, this)));

		m_rotate = rotate;
	}

	~MovableKeyListener() = default;

private:
	void bigger()
	{
		m_movable.scale(glm::vec3(m_scale));
		fprintf(stdout, "<key_callback> + pressed\n");
	}

	void smaller()
	{
		m_movable.scale(glm::vec3(1.0f / m_scale));
		fprintf(stdout, "<key_callback> - pressed\n");
	}

	void rotateX()
	{
		m_movable.rotate(glm::vec3(1.0f, 0, 0), m_rotate);
	}

	void rotateY()
	{
		m_movable.rotate(glm::vec3(0, 1.0f, 0), m_rotate);
	}

	void rotateZ()
	{
		m_movable.rotate(glm::vec3(0, 0, 1.0f), m_rotate);
	}

	MovableObject& m_movable;
	float m_rotate;
	float m_scale;
};

template <class S>
class ShapeKeyListener : public MovableKeyListener
{
public:
	ShapeKeyListener(GLFWwindow* window, S& shape, float rotate = 1.0f, float scale = 1.01f)
		: MovableKeyListener(window, shape, rotate, scale),
		  m_shape(shape)
	{
		registerCallback(window, GLFW_KEY_F, GLFW_PRESS, KeyCallbackFunc(std::bind(&ShapeKeyListener::wireframe, this)));
	}

	~ShapeKeyListener() = default;

private:
	void wireframe()
	{
		m_wireframe = !m_wireframe;
		m_shape.wireframe(m_wireframe);
	}

	S& m_shape;
	bool m_wireframe;
};

template <class S>
class SurfaceKeyListener : public MovableKeyListener
{
public:
	SurfaceKeyListener(GLFWwindow* window, S& surface, float rotate = 1.0f, float scale = 1.01f)
		: MovableKeyListener(window, surface, rotate, scale),
		  m_surface(surface)
	{
		registerCallback(window, GLFW_KEY_F, GLFW_PRESS, KeyCallbackFunc(std::bind(&SurfaceKeyListener::wireframe, this)));
	}

	~SurfaceKeyListener() = default;

private:
	void wireframe()
	{
		m_wireframe = !m_wireframe;
		m_surface.wireframe(m_wireframe);
	}

	S& m_surface;
	bool m_wireframe;
};

template <class C>
class ContourKeyListener : public MovableKeyListener
{
public:
	ContourKeyListener(GLFWwindow* window, C& contour, float rotate = 1.0f, float scale = 1.01f, float iso = 0.05f)
		: MovableKeyListener(window, contour, rotate, scale),
		  m_iso(iso),
		  m_contour(contour)
	{
		registerCallback(window, GLFW_KEY_U, GLFW_PRESS, KeyCallbackFunc(std::bind(&ContourKeyListener::isoUp, this)));
		registerCallback(window, GLFW_KEY_U, GLFW_REPEAT, KeyCallbackFunc(std::bind(&ContourKeyListener::isoUp, this)));
		registerCallback(window, GLFW_KEY_I, GLFW_PRESS, KeyCallbackFunc(std::bind(&ContourKeyListener::isoDown, this)));
		registerCallback(window, GLFW_KEY_I, GLFW_REPEAT, KeyCallbackFunc(std::bind(&ContourKeyListener::isoDown, this)));
	}

	~ContourKeyListener() = default;

private:
	void isoUp()
	{
		float isoValue = m_contour.getIsoValue() + m_iso;
		m_contour.setIsoValue(isoValue);
	}

	void isoDown()
	{
		float isoValue = m_contour.getIsoValue() - m_iso;
		m_contour.setIsoValue(isoValue);
	}

	C& m_contour;
	float m_iso;
};