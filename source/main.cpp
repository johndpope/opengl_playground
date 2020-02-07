#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <box.h>
#include <camera.h>
#include <light_shape.h>
#include <key_listener.h>

std::map<GLFWwindow*, std::map<int, std::vector<KeyListener::KeyCallbackFuncArgs>>> KeyListener::m_windowKeyMap;

const GLuint SCR_WIDTH = 1200;
const GLuint SCR_HEIGHT = 1200;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void GLAPIENTRY
messageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s source = 0x%x, type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		source, type, severity, message);
}

GLFWwindow* initWindow()
{
	GLFWwindow* window;
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Exercise 3 - Textures, Shading, Lighting", NULL, NULL);
	if (NULL == window)
	{
		fprintf(stderr, "Failed to create GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	GLenum glerr = glewInit();
	if (GLEW_OK != glerr)
	{
		fprintf(stderr, "glewInit Error: %s\n", glewGetErrorString(glerr));
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);

	glDebugMessageCallback(messageCallback, 0);

	return window;
}

int main(int argc, char **argv)
{
	GLFWwindow* window = initWindow();

	std::string textureNames[NUM_BOX_SIDES] = {
		"texture\\nyan.jpg",
		"texture\\cat.jpg",
		"texture\\woman.jpg",
		"texture\\sax.jpg",
		"texture\\gandalf.jpg",
		"texture\\table.jpg",
	};

	PerspectiveCamera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, 0.1f, 50.0f);
	camera.init();
	camera.translate(glm::vec3(0, 0, 10.0f));
	camera.lookAt(glm::vec3(0, 0, 0));
	CameraKeyListener(window, camera);

	UniqueTextureBox box1(textureNames, glm::vec3(1.0f));
	box1.init();
	box1.translate(glm::vec3(-0.5f, 0, 0));
	ShapeKeyListener<UniqueTextureBox>(window, box1);

	ColorBox box2(glm::vec3(0.5f, 1.0f, 0.25f), glm::vec3(1.0f));
	box2.init();
	box2.translate(glm::vec3(0.5f, 0, 0));
	ShapeKeyListener<ColorBox>(window, box2);

	BasicColorBox box3(glm::vec3(1.0f, 0.0, 0.25f), glm::vec3(1.0f));
	box3.init();
	box3.translate(glm::vec3(0.0, 1.0f, 0));
	ShapeKeyListener<BasicColorBox>(window, box3);

	LightBox light;
	light.init();
	light.translate(glm::vec3(0.0f, 0, 2.0f));
	light.orbit(glm::vec3(0, 0, 1.0f), glm::vec3(0, 0, 2.0f), 1.0f, 3.0f);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.update();

		box1.update(camera, light);
		box2.update(camera, light);
		box3.update(camera, light);

		light.update(camera);

		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
