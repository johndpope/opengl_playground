#pragma once

#include <glew.h>
#include <glfw3.h>
#include <map>
#include <vector>
#include <camera.h>
#include <shape.h>
#include <surface.h>
#include <contour.h>
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

class CameraKeyListener
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

class MovableKeyListener
{
public:
	MovableKeyListener(GLFWwindow* window, MovableObject& movable, float rotate = 1.0f, float scale = 1.01f);
	~MovableKeyListener() = default;

private:
	void bigger();
	void smaller();
	void rotateX();
	void rotateY();
	void rotateZ();

	MovableObject& m_movable;
	float m_rotate;
	float m_scale;
};

class ShapeKeyListener : public MovableKeyListener
{
public:
	ShapeKeyListener(GLFWwindow* window, Shape& shape, float rotate = 1.0f, float scale = 1.01f);
	~ShapeKeyListener() = default;

private:
	void wireframe();

	Shape& m_shape;
	bool m_wireframe;
};

class SurfaceKeyListener : public MovableKeyListener
{
public:
	SurfaceKeyListener(GLFWwindow* window, Surface& surface, float rotate = 1.0f, float scale = 1.01f);
	~SurfaceKeyListener() = default;

private:
	void wireframe();

	Surface& m_surface;
	bool m_wireframe;
};

class ContourKeyListener : public MovableKeyListener
{
public:
	ContourKeyListener(GLFWwindow* window, Contour& contour, float rotate = 1.0f, float scale = 1.01f, float iso = 0.01f);
	~ContourKeyListener() = default;

private:
	void isoUp();
	void isoDown();

	Contour& m_contour;
	float m_iso;
};