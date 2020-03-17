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
	KeyListener::registerCallback(window, GLFW_KEY_A, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkLeft, this)));
	KeyListener::registerCallback(window, GLFW_KEY_A, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkLeft, this)));
	KeyListener::registerCallback(window, GLFW_KEY_A, GLFW_RELEASE, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkLeft, this)));
	KeyListener::registerCallback(window, GLFW_KEY_W, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkForward, this)));
	KeyListener::registerCallback(window, GLFW_KEY_W, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkForward, this)));
	KeyListener::registerCallback(window, GLFW_KEY_W, GLFW_RELEASE, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkForward, this)));
	KeyListener::registerCallback(window, GLFW_KEY_S, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkBackward, this)));
	KeyListener::registerCallback(window, GLFW_KEY_S, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkBackward, this)));
	KeyListener::registerCallback(window, GLFW_KEY_S, GLFW_RELEASE, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkBackward, this)));
	KeyListener::registerCallback(window, GLFW_KEY_D, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkRight, this)));
	KeyListener::registerCallback(window, GLFW_KEY_D, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkRight, this)));
	KeyListener::registerCallback(window, GLFW_KEY_D, GLFW_RELEASE, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::walkRight, this)));
	KeyListener::registerCallback(window, GLFW_KEY_J, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::rotateLeft, this)));
	KeyListener::registerCallback(window, GLFW_KEY_J, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::rotateLeft, this)));
	KeyListener::registerCallback(window, GLFW_KEY_J, GLFW_RELEASE, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::rotateLeft, this)));
	KeyListener::registerCallback(window, GLFW_KEY_L, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::rotateRight, this)));
	KeyListener::registerCallback(window, GLFW_KEY_L, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::rotateRight, this)));
	KeyListener::registerCallback(window, GLFW_KEY_L, GLFW_RELEASE, KeyListener::KeyCallbackFunc(std::bind(&CameraKeyListener::rotateRight, this)));
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

MovableKeyListener::MovableKeyListener(GLFWwindow* window, MovableObject& movable, float rotate, float scale)
    : m_movable(movable),
      m_rotate(rotate),
      m_scale(scale)
{
    KeyListener::registerCallback(window, GLFW_KEY_EQUAL, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::bigger, this)));
    KeyListener::registerCallback(window, GLFW_KEY_EQUAL, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::bigger, this)));
    KeyListener::registerCallback(window, GLFW_KEY_MINUS, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::smaller, this)));
    KeyListener::registerCallback(window, GLFW_KEY_MINUS, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::smaller, this)));
    KeyListener::registerCallback(window, GLFW_KEY_X, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::rotateX, this)));
    KeyListener::registerCallback(window, GLFW_KEY_X, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::rotateX, this)));
    KeyListener::registerCallback(window, GLFW_KEY_Y, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::rotateY, this)));
    KeyListener::registerCallback(window, GLFW_KEY_Y, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::rotateY, this)));
    KeyListener::registerCallback(window, GLFW_KEY_Z, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::rotateZ, this)));
    KeyListener::registerCallback(window, GLFW_KEY_Z, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MovableKeyListener::rotateZ, this)));

    m_rotate = rotate;
}

void MovableKeyListener::bigger()
{
    m_movable.scale(glm::vec3(m_scale));
    fprintf(stdout, "<key_callback> + pressed\n");
}

void MovableKeyListener::smaller()
{
    m_movable.scale(glm::vec3(1.0f / m_scale));
    fprintf(stdout, "<key_callback> - pressed\n");
}

void MovableKeyListener::rotateX()
{
    m_movable.rotate(glm::vec3(1.0f, 0, 0), m_rotate);
}

void MovableKeyListener::rotateY()
{
    m_movable.rotate(glm::vec3(0, 1.0f, 0), m_rotate);
}

void MovableKeyListener::rotateZ()
{
    m_movable.rotate(glm::vec3(0, 0, 1.0f), m_rotate);
}

ShapeKeyListener::ShapeKeyListener(GLFWwindow* window, Shape& shape, float rotate, float scale)
    : MovableKeyListener(window, shape, rotate, scale),
      m_shape(shape)
{
    KeyListener::registerCallback(window, GLFW_KEY_F, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&ShapeKeyListener::wireframe, this)));
}

void ShapeKeyListener::wireframe()
{
    m_wireframe = !m_wireframe;
    m_shape.wireframe(m_wireframe);
}

SurfaceKeyListener::SurfaceKeyListener(GLFWwindow* window, Surface& surface, float rotate, float scale)
    : MovableKeyListener(window, surface, rotate, scale),
      m_surface(surface)
{
    KeyListener::registerCallback(window, GLFW_KEY_F, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&SurfaceKeyListener::wireframe, this)));
}

void SurfaceKeyListener::wireframe()
{
    m_wireframe = !m_wireframe;
    m_surface.wireframe(m_wireframe);
}

ContourKeyListener::ContourKeyListener(GLFWwindow* window, Contour& contour, float rotate, float scale, float iso)
    : MovableKeyListener(window, contour, rotate, scale),
      m_iso(iso),
      m_contour(contour)
{
    KeyListener::registerCallback(window, GLFW_KEY_U, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&ContourKeyListener::isoUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_U, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&ContourKeyListener::isoUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_I, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&ContourKeyListener::isoDown, this)));
    KeyListener::registerCallback(window, GLFW_KEY_I, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&ContourKeyListener::isoDown, this)));
}

void ContourKeyListener::isoUp()
{
    float isoValue = m_contour.getIsoValue() + m_iso;
    m_contour.setIsoValue(isoValue);
}

void ContourKeyListener::isoDown()
{
    float isoValue = m_contour.getIsoValue() - m_iso;
    m_contour.setIsoValue(isoValue);
}

MeshKeyListener::MeshKeyListener(GLFWwindow* window, Mesh& mesh, float rotate, float scale, float iso)
    : MovableKeyListener(window, mesh, rotate, scale),
      m_iso(iso),
      m_mesh(mesh)
{
    KeyListener::registerCallback(window, GLFW_KEY_N, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshKeyListener::isoUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_N, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MeshKeyListener::isoUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_M, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshKeyListener::isoDown, this)));
    KeyListener::registerCallback(window, GLFW_KEY_M, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MeshKeyListener::isoDown, this)));
	KeyListener::registerCallback(window, GLFW_KEY_F, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshKeyListener::wireframe, this)));
}

void MeshKeyListener::isoUp()
{
    float isoValue = m_mesh.getIsoValue() + m_iso;
    m_mesh.setIsoValue(isoValue);
}

void MeshKeyListener::isoDown()
{
    float isoValue = m_mesh.getIsoValue() - m_iso;
    m_mesh.setIsoValue(isoValue);
}

void MeshKeyListener::wireframe()
{
    m_wireframe = !m_wireframe;
    m_mesh.wireframe(m_wireframe);
}

MeshContourKeyListener::MeshContourKeyListener(GLFWwindow* window, Mesh& mesh, Contour& contour, float rotate, float scale, float iso, float slice)
    : MovableKeyListener(window, contour, rotate, scale),
      m_iso(iso),
	  m_slice(slice),
      m_mesh(mesh),
	  m_contour(contour),
	  m_meshKeyListener(new MovableKeyListener(window, mesh))
{
    KeyListener::registerCallback(window, GLFW_KEY_U, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::sliceUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_U, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::sliceUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_I, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::sliceDown, this)));
    KeyListener::registerCallback(window, GLFW_KEY_I, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::sliceDown, this)));
    KeyListener::registerCallback(window, GLFW_KEY_N, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::isoUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_N, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::isoUp, this)));
    KeyListener::registerCallback(window, GLFW_KEY_M, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::isoDown, this)));
    KeyListener::registerCallback(window, GLFW_KEY_M, GLFW_REPEAT, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::isoDown, this)));
	KeyListener::registerCallback(window, GLFW_KEY_F, GLFW_PRESS, KeyListener::KeyCallbackFunc(std::bind(&MeshContourKeyListener::wireframe, this)));
}

void MeshContourKeyListener::isoUp()
{
    float isoValue = m_mesh.getIsoValue() + m_iso;
    m_mesh.setIsoValue(isoValue);
	m_contour.setIsoValue(isoValue);
}

void MeshContourKeyListener::isoDown()
{
    float isoValue = m_mesh.getIsoValue() - m_iso;
    m_mesh.setIsoValue(isoValue);
	m_contour.setIsoValue(isoValue);
}

void MeshContourKeyListener::sliceUp()
{
	m_contour.incrementHeight(m_slice);
}

void MeshContourKeyListener::sliceDown()
{
	m_contour.incrementHeight(-m_slice);
}

void MeshContourKeyListener::wireframe()
{
    m_wireframe = !m_wireframe;
    m_mesh.wireframe(m_wireframe);
}