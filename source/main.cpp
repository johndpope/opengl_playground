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

float calculation3DRadius(float x, float y, float z)
{
	return sqrt(x * x + y * y + z * z);
}

float calculation3DSinc(float x, float y, float z)
{
	return (sinf(10 * x) * sinf(10 * y) * sinf(10 * z)) / (1000 * x * y * z);
}

glm::vec4 color(float value)
{
    float d = value * 6;
    d = d < 0.8f ? 0.8f : (d > 5.2f ? 5.2f : d);

	float r = fmax(0, (3 - fabs(d - 4) - fabs(d - 5)) / 12.0f);
	float g = fmax(0, (4 - fabs(d - 2) - fabs(d - 4)) / 12.0f);
	float b = fmax(0, (3 - fabs(d - 1) - fabs(d - 2)) / 12.0f);

	glm::vec3 rgb = glm::normalize(glm::vec3(r, g, b));

	return glm::vec4(rgb, 0.10f);
}

glm::vec4 bonzaiColor(float value)
{
	float d = value * 255.0f;
	float r = fmin(1.0f, fmax(0, 0.396f * (d - 30.0f) / 48.0f));
	float g = fmin(1.0f, fmax(0, 1.0f + 0.75f * (30.0f - d) / 48.0f));
	float b = fmin(1.0f, fmax(0, 0.129f * (d - 30.0f) / 48.0f));
	float a = fmin(1.0f, fmax(0, 0.75f * (d - 10.5f) / 48.0f));

	glm::vec3 rgb = glm::normalize(glm::vec3(r, g, b));

	return glm::vec4(rgb, a);
}

void bonzaiMain(GLFWwindow* window)
{
	PvmGrid3D grid3D("datasets\\Bonsai2-HI.pvm");
	Mesh mesh(grid3D, bonzaiColor);
	mesh.init();
	mesh.setIsoValue(78.0f);
	MeshKeyListener meshKeyListener = MeshKeyListener(window, mesh);

	Mesh mesh3(grid3D, bonzaiColor);
	mesh3.init();
	mesh3.setIsoValue(30.0f);
	MeshKeyListener meshKeyListener3 = MeshKeyListener(window, mesh3);

	Mesh mesh2(grid3D, bonzaiColor);
	mesh2.init();
	mesh2.setIsoValue(210.0f);
	MeshKeyListener meshKeyListener2 = MeshKeyListener(window, mesh2);

	LightBox light(glm::vec3(5.0f));
	light.init();
	light.orbit(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0.0f), 130.0f, 2.0f, 0.1f);

	PerspectiveCamera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, 0.1f, 500.0f);
	camera.init();
	camera.translate(glm::vec3(0, 0, 300.0f));
	camera.lookAt(glm::vec3(0, 0, 0));
	CameraKeyListener cameraKeyListener = CameraKeyListener(window, camera);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.update();
		mesh2.update(camera, light);
		mesh.update(camera, light);
		mesh3.update(camera, light);
		light.update(camera);

		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void testMain(GLFWwindow* window)
{
   	CalculateGrid3D grid3DRadius(calculation3DRadius, 40, 40, 40, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
	Mesh meshRadius(grid3DRadius, color);
	meshRadius.init();
	meshRadius.setIsoValue(0.5f);

	CalculateGrid3D grid3DSinc(calculation3DSinc, 40, 40, 40, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
	Mesh meshSinc(grid3DSinc, color);
	meshSinc.init();
	meshSinc.setIsoValue(0.5f);

	Plane slicePlane = { 1.0f, 0, 1.0f, 0 };

	SliceGrid2D grid2DRadius(grid3DRadius, slicePlane, 50, 50, -1.0f, -1.0f, 1.0f, 1.0f);
	ColorContour contourRadius(grid2DRadius, glm::vec4(0, 0, 0, 1.0f));
	contourRadius.init();
	contourRadius.setIsoValue(0.5f);
	MeshContourKeyListener meshContourKeyListenerRadius = MeshContourKeyListener(window, meshRadius, contourRadius);

	SliceGrid2D grid2DSinc(grid3DSinc, slicePlane, 50, 50, -1.0f, -1.0f, 1.0f, 1.0f);
	ColorContour contourSinc(grid2DSinc, glm::vec4(0, 0, 0, 1.0f));
	contourSinc.init();
	contourSinc.setIsoValue(0.5f);
	MeshContourKeyListener meshContourKeyListenerSinc = MeshContourKeyListener(window, meshSinc, contourSinc);

	LightBox light;
	light.init();
	light.orbit(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0.0f), 2.0f, 2.0f, 0.1f);

	PerspectiveCamera camera(SCR_WIDTH, SCR_HEIGHT, 45.0f, 0.1f, 50.0f);
	camera.init();
	camera.translate(glm::vec3(0, 0, 3.0f));
	camera.lookAt(glm::vec3(0, 0, 0));
	CameraKeyListener cameraKeyListener = CameraKeyListener(window, camera);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.update();
		contourRadius.update(camera);
		contourSinc.update(camera);
		meshRadius.update(camera, light);
		meshSinc.update(camera, light);
		light.update(camera);

		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char **argv)
{
	GLFWwindow* window = initWindow();

	if (argc > 1)
	{
		std::cout << "Using bonzai tree visualization main" << std::endl;
		bonzaiMain(window);
	}
	else
	{
		std::cout << "Using test visualization main" << std::endl;
		std::cout << "Run with any argument to use the bonzai tree visualization (extremely slow loading...)" << std::endl;
		testMain(window);
	}

	glfwTerminate();
	return 0;
}
