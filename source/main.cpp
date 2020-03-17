#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <box.h>
#include <camera.h>
#include <contour.h>
#include <light_shape.h>
#include <key_listener.h>
#include <sphere.h>
#include <grid.h>
#include <ddsbase.h>
#include <algorithm>

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

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 4", NULL, NULL);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDebugMessageCallback(messageCallback, 0);

	return window;
}

float calculation2D(float x, float y)
{
	return (sinf(10 * x) * sinf(10 * y)) / (100 * x * y);
}

float calculation3D(float x, float y, float z)
{
	return sqrt(x * x + y * y + z * z);
	//return (sinf(10 * x) * sinf(10 * y) * sinf(10 * z)) / (1000 * x * y * z);
}

glm::vec4 color(float value)
{
    float d = value * 6;
    d = d < 0.8f ? 0.8f : (d > 5.2f ? 5.2f : d);

	float r = fmax(0, (3 - fabs(d - 4) - fabs(d - 5)) / 12.0f);
	float g = fmax(0, (4 - fabs(d - 2) - fabs(d - 4)) / 12.0f);
	float b = fmax(0, (3 - fabs(d - 1) - fabs(d - 2)) / 12.0f);

	glm::vec3 rgb = glm::normalize(glm::vec3(r, g, b));

	return glm::vec4(rgb, 0.25f);
}

int main(int argc, char **argv)
{
    unsigned char *volume;
    unsigned int components;
    unsigned int depth;
    unsigned int width;
    unsigned int height;

    float scalex;
    float scaley;
    float scalez;

    volume = readPVMvolume(
        "datasets\\Bonsai2-HI.pvm",
        &width,
        &height,
        &depth,
        &components,
        &scalex,
        &scaley,
        &scalez,
        NULL, NULL, NULL, NULL
    );

    free(volume);

	GLFWwindow* window = initWindow();

    CalculateGrid3D grid3D(calculation3D, 30, 30, 30, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
	Mesh mesh(grid3D, color);
	mesh.init();
	mesh.setIsoValue(0.5f);

	Plane slicePlane = { 1.0f, 1.0f, 0, 0 };
	SliceGrid2D grid2D(grid3D, slicePlane, 30, 30, -1.0f, -1.0f, 1.0f, 1.0f);
	ColorContour contour(grid2D, glm::vec4(0, 0, 0, 1.0f));
	contour.init();
	contour.setIsoValue(0.5f);
	MeshContourKeyListener meshContourKeyListener = MeshContourKeyListener(window, mesh, contour);

	LightBox light;
	light.init();
	light.orbit(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0.0f), 2.0f, 2.0f, 0.1f);

	PerspectiveCamera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, 0.1f, 50.0f);
	camera.init();
	camera.translate(glm::vec3(0, 0, 10.0f));
	camera.lookAt(glm::vec3(0, 0, 0));
	CameraKeyListener cameraKeyListener = CameraKeyListener(window, camera);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.update();
		contour.update(camera);
        mesh.update(camera);
		light.update(camera);

		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
