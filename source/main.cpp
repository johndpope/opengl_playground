#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <box.h>
#include <camera.h>
#include <contour.h>
#include <light_shape.h>
#include <key_listener.h>
#include <uniform_grid.h>

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

float calculation1(float x, float y)
{
    return std::exp(-(x*x + y*y));
}

float calculation2(float x, float y)
{
	return sinf(1.0f / (x*x + y * y));
}

int main(int argc, char **argv)
{
	GLFWwindow* window = initWindow();

    UniformGrid grid(calculation1, 300, 300, -3.0f, -3.0f, 3.0f, 3.0f);
    grid.init();
	SurfaceKeyListener surfaceKeyListener = SurfaceKeyListener(window, grid);

	ColorContour contour(grid);
	contour.init();
	ContourKeyListener contourKeyListener = ContourKeyListener(window, contour);

	LightBox light;
	light.init();
	light.orbit(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0.0f), 2.0f, 2.0f, 0.0f);

	PerspectiveCamera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, 0.1f, 50.0f);
	camera.init();
	camera.translate(glm::vec3(0, 0, 10.0f));
	camera.lookAt(glm::vec3(0, 0, 0));
	CameraKeyListener cameraKeyListener = CameraKeyListener(window, camera);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.update();
		grid.update(camera, light);
		contour.update(camera, light);
		light.update(camera);

		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
