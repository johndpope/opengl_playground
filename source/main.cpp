#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <box.h>
#include <camera.h>
#include <light_shape.h>
#include <key_listener.h>

std::map<GLFWwindow*, std::map<int, std::vector<KeyListener::KeyCallbackFunc>>> KeyListener::m_windowKeyMap;

const GLuint SCR_WIDTH = 1200;
const GLuint SCR_HEIGHT = 1200;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	GLenum glerr = glewInit();
	if (GLEW_OK != glerr)
	{
		fprintf(stderr, "glewInit Error: %s\n", glewGetErrorString(glerr));
	}

	glEnable(GL_DEPTH_TEST);

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
	UniqueTextureBox controlBox(textureNames, glm::vec3(1.0f));
	LightBox light;

	controlBox.init();
	light.init();
	camera.init();

	camera.translate(glm::vec3(0, 0, 10.0f));
	camera.lookAt(glm::vec3(0, 0, 0));
	light.translate(glm::vec3(-2.0f, -2.0f, 2.0f));
	light.translateTo(glm::vec3(2.0f, 2.0f, 2.0f), 10.0f);

	CameraKeyListener(window, camera);
	ShapeKeyListener<TextureShader>(window, controlBox);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.4f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//light.update(camera);
		controlBox.update(camera, light);

		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
