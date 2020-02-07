#include <key_listener.h>

std::map<GLFWwindow*, std::map<int, std::vector<KeyListener::KeyCallbackFuncArgs>>> KeyListener::m_windowKeyMap;

void KeyListener::registerCallback(GLFWwindow* window, int key, int action, KeyCallbackFunc func)
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

void KeyListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

CameraKeyListener::CameraKeyListener(GLFWwindow* window, Camera& camera, float rotateSpeed, float translateSpeed, float scale)
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

void CameraKeyListener::rotateLeft()
{
	rotate(-m_rotateSpeed);
}

void CameraKeyListener::rotateRight()
{
	rotate(m_rotateSpeed);
}

void CameraKeyListener::walkLeft()
{
	translate(glm::vec3(1.0f, 0, 0), m_translateSpeed);
}

void CameraKeyListener::walkRight()
{
	translate(glm::vec3(1.0f, 0, 0), -m_translateSpeed);
}

void CameraKeyListener::walkForward()
{
	translate(glm::vec3(0, 0, 1.0f), m_translateSpeed);
}

void CameraKeyListener::walkBackward()
{
	translate(glm::vec3(0, 0, 1.0f), -m_translateSpeed);
}

void CameraKeyListener::rotate(float angle)
{
	glm::vec3 translation = m_camera.translation();
	m_camera.translate(-translation);
	m_camera.rotate(glm::vec3(0, 1.0f, 0), angle);
	m_camera.translate(translation);
}

void CameraKeyListener::translate(glm::vec3 moveDirection, float speed)
{
	glm::vec3 translation = glm::normalize(glm::vec3(m_camera.rotation() * glm::vec4(moveDirection, 1.0f)));
	translation *= speed;
	m_camera.translate(translation);
}