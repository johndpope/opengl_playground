// UI control headers
#include <glew.h>
#include <glfw3.h>

// GLM GL Math header library
#include <glm.hpp>
#include ".\gtc\matrix_transform.hpp"
#include ".\gtc\matrix_access.hpp"
#include ".\gtc\matrix_inverse.hpp"

// Shader wrapper C++ class
#include <cshader.h>

// Custom headers
#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include <box.h>

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

const GLuint SCR_WIDTH = 1200;
const GLuint SCR_HEIGHT = 1200;

float can_cube_verts_cols[] = {

	//+Z
	-1.0f, -1.0f, 1.0f,	 0.0,  0.0 , 0.0, 0.0, -1.0f, // v0
	1.0f, -1.0f, 1.0f,	 1.0f, 0.0 , 0.0, 0.0, -1.0f, // v1
	1.0f, 1.0f, 1.0f,	 1.0f, 1.0f, 0.0, 0.0, -1.0f, // v2

	-1.0f, -1.0f, 1.0f,	 0.0,  0.0 , 0.0, 0.0, -1.0f, // v0
	1.0f, 1.0f, 1.0f,	 1.0f, 1.0f, 0.0, 0.0, -1.0f, // v2
	-1.0f, 1.0f, 1.0f,	 0.0,  1.0f, 0.0, 0.0, -1.0f, // v3

	//+X
	1.0f, -1.0f, 1.0f,	 0.0,  0.0 , 1.0f, 0.0, 0.0, // v1
	1.0f, -1.0f, -1.0f,	 1.0f, 0.0 , 1.0f, 0.0, 0.0, // v4
	1.0f, 1.0f, -1.0f,	 1.0f, 1.0f, 1.0f, 0.0, 0.0, // v7

	1.0f, -1.0f, 1.0f,	 0.0,  0.0 , 1.0f, 0.0, 0.0, // v1
	1.0f, 1.0f, -1.0f,	 1.0f, 1.0f, 1.0f, 0.0, 0.0, // v7
	1.0f, 1.0f, 1.0f,	 0.0,  1.0f, 1.0f, 0.0, 0.0, // v2

	//+Y
	-1.0f, 1.0f, 1.0f,	 0.0,  0.0 , 0.0, 1.0f, 0.0, // v3
	1.0f, 1.0f, 1.0f,	 1.0f, 0.0 , 0.0, 1.0f, 0.0, // v2
	1.0f, 1.0f, -1.0f,	 1.0f, 1.0f, 0.0, 1.0f, 0.0, // v7

	-1.0f, 1.0f, 1.0f,	 0.0,  0.0 , 0.0, 1.0f, 0.0, // v3
	1.0f, 1.0f, -1.0f,	 1.0f, 1.0f, 0.0, 1.0f, 0.0, // v7
	-1.0f, 1.0f, -1.0f,	 0.0,  1.0f, 0.0, 1.0f, 0.0, // v6

	//-Z
	1.0f, -1.0f, -1.0f,	 0.0,  0.0 , 0.0, 0.0, -1.0f, // v4
	-1.0f, -1.0f, -1.0f, 1.0f, 0.0 , 0.0, 0.0, -1.0f, // v5
	-1.0f, 1.0f, -1.0f,	 1.0f, 1.0f, 0.0, 0.0, -1.0f, // v6

	1.0f, -1.0f, -1.0f,	 0.0,  0.0 , 0.0, 0.0, -1.0f, // v4
	-1.0f, 1.0f, -1.0f,	 1.0f, 1.0f, 0.0, 0.0, -1.0f, // v6
	1.0f, 1.0f, -1.0f,	 0.0,  1.0f, 0.0, 0.0, -1.0f, // v7

	//-X
	-1.0f, -1.0f, -1.0f, 0.0,  0.0 , -1.0f, 0.0, 0.0, // v5
	-1.0f, -1.0f, 1.0f,	 1.0f, 0.0 , -1.0f, 0.0, 0.0, // v0
	-1.0f, 1.0f, 1.0f,	 1.0f, 1.0f, -1.0f, 0.0, 0.0, // v3

	-1.0f, -1.0f, -1.0f, 0.0,  0.0 , -1.0f, 0.0, 0.0, // v5
	-1.0f, 1.0f, 1.0f,	 1.0f, 1.0f, -1.0f, 0.0, 0.0, // v3
	-1.0f, 1.0f, -1.0f,	 0.0,  1.0f, -1.0f, 0.0, 0.0, // v6

	//-Y
	-1.0f, -1.0f, 1.0f,  0.0,  0.0 , 0.0, -1.0f, 0.0, // v0
	-1.0f, -1.0f, -1.0f, 1.0f, 0.0 , 0.0, -1.0f, 0.0, // v5
	1.0f, -1.0f, -1.0f,	 1.0f, 1.0f, 0.0, -1.0f, 0.0, // v4

	-1.0f, -1.0f, 1.0f,	 0.0,  0.0 , 0.0, -1.0f, 0.0, // v0
	1.0f, -1.0f, -1.0f,	 1.0f, 1.0f, 0.0, -1.0f, 0.0, // v4
	1.0f, -1.0f, 1.0f,	 0.0,  1.0f, 0.0, -1.0f, 0.0  // v1
};

// Define framebuffer callback function
// HERE
// GLFW Callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// GLOBAL DEFS = BAD in general
// variables which get modified in keyboard callback function and affect render loop
glm::mat4 g_model;
glm::mat4 g_view;

float g_rotate_angle = 45.0f;
float g_scale_factor = 1.0f;
float g_cam_z = 10.00f;  // can't have the lookdir vector (lookpt - eyept) = 0.
GLenum gl_polygon_mode = GL_FILL;

#define NUM_TEXTURES 6
GLuint g_textures[NUM_TEXTURES];
Material g_materials[NUM_TEXTURES];

// Keyboard callback function, handles keypress events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		break;
	case GLFW_KEY_W: // toggle between wire-frame and filled polygon rendering
		if (action == GLFW_PRESS) {
			if (gl_polygon_mode == GL_FILL)
				gl_polygon_mode = GL_LINE;
			else
				gl_polygon_mode = GL_FILL;
		}
		break;
	case GLFW_KEY_P: // "Plus": shift camera position in +z-dir by 0.1 units.
		if (action == GLFW_PRESS) {
			g_cam_z += 0.1f;
			g_view = glm::lookAt(glm::vec3(0.0f, 0.0f, g_cam_z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			fprintf(stderr, "<key_callback> P pressed: g_cam_z = %f\n", g_cam_z);
		}
		break;
	case GLFW_KEY_M: // "Minus": shift camera position in -z-dir by 0.1 units.
		if (action == GLFW_PRESS) {
			g_cam_z -= 0.1f;
			g_view = glm::lookAt(glm::vec3(0.0f, 0.0f, g_cam_z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			fprintf(stderr, "<key_callback> M pressed: g_cam_z = %f\n", g_cam_z);
		}
		break;
	case GLFW_KEY_B: // "Bigger": double the scale factor
		if (action == GLFW_PRESS) {
			if(g_scale_factor < 1.0f)
				g_scale_factor = 1.0f;
			g_scale_factor *= 2.0f;
			g_model = glm::scale(g_model, glm::vec3(g_scale_factor));
			fprintf(stderr, "<key_callback> B pressed: g_scale_factor = %f\n", g_scale_factor);
		}
		break;
	case GLFW_KEY_S: // "Smaller": half the scale factor
		if (action == GLFW_PRESS) {
			if(g_scale_factor > 1.0f)
				g_scale_factor = 1.0f;
			g_scale_factor *= 0.5f;
			g_model = glm::scale(g_model, glm::vec3(g_scale_factor));
			fprintf(stderr, "<key_callback> S pressed: g_scale_factor = %f\n", g_scale_factor);
		}
		break;
	case GLFW_KEY_X:  // Rotate by g_rotate_angle degress about X axis
		if (action == GLFW_PRESS) {
			g_model = glm::rotate(g_model, (float)glm::radians(g_rotate_angle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		break;
	case GLFW_KEY_Y: // Rotate by g_rotate_angle degress about Y axis
		if (action == GLFW_PRESS) {
			g_model = glm::rotate(g_model, (float)glm::radians(g_rotate_angle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		break;
	case GLFW_KEY_Z: // Rotate by g_rotate_angle degress about Z axis
		if (action == GLFW_PRESS) {
			g_model = glm::rotate(g_model, (float)glm::radians(g_rotate_angle), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		break;
	default:
		break;
	}
}

// Define init( ) function
// HERE
// Interleaved AoS Buffer method
GLuint init1(GLuint* vao)
{
	// Load & compile/link shaders and use the resulting shader program
	CShader myShaderWrap("shader\\textureVertexShader.glsl", "shader\\textureFragmentShader.glsl");
	myShaderWrap.use();

	// Create a vertex array object
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	// Create and initialize a vertex buffer object using separate SoA buffer
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(can_cube_verts_cols), can_cube_verts_cols, GL_STATIC_DRAW);

	// Initialize the vertex position attribute dynamically from the vertex shader variable name
	GLuint loc = glGetAttribLocation(myShaderWrap.getProgram(), "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // use interleaved stride

	GLuint tex = glGetAttribLocation(myShaderWrap.getProgram(), "vTexCoord");
	glEnableVertexAttribArray(tex);
	glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // interleaved stride & offset

	GLuint norm = glGetAttribLocation(myShaderWrap.getProgram(), "vNormal");
	glEnableVertexAttribArray(norm);
	glVertexAttribPointer(norm, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // interleaved stride & offset

	std::string textureNames[NUM_TEXTURES] = {
		"texture\\nyan.jpg",
		"texture\\cat.jpg",
		"texture\\woman.jpg",
		"texture\\sax.jpg",
		"texture\\gandalf.jpg",
		"texture\\table.jpg",
	};

	// Setup textures
	glGenTextures(NUM_TEXTURES, g_textures);
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, g_textures[i]);

		// set the texture wrapping & filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// load the texture image array using STB
		int width, height, nrChannels;
		unsigned char *data = stbi_load(textureNames[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}

		g_materials[i].shininess = rand()/(float)RAND_MAX;
		g_materials[i].ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);// glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		g_materials[i].diffuseColor = glm::vec3(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX);
		g_materials[i].specularColor = glm::vec3(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX);
	}

	glBindVertexArray(0); // unbind

	return myShaderWrap.getProgram();
}

// Define main( ) entry point function
// - should contain GLFW and GLEW initialization steps
// - call init( ) function
// - enter main render loop and do OpenGL drawing
// HERE
int main(int argc, char **argv)
{
	GLFWwindow* window;  // encapsulates both window and context
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
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	// hook up (register) the GLFW framebuffer and keyboard callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	GLenum glerr = glewInit();
	if (GLEW_OK != glerr)
	{
		fprintf(stderr, "glewInit Error: %s\n", glewGetErrorString(glerr));
	}

	glEnable(GL_DEPTH_TEST);

	std::string textureNames[NUM_TEXTURES] = {
		"texture\\nyan.jpg",
		"texture\\cat.jpg",
		"texture\\woman.jpg",
		"texture\\sax.jpg",
		"texture\\gandalf.jpg",
		"texture\\table.jpg",
	};

	UniqueTextureBox test(textureNames, glm::vec3(1.0f));
	//ColorBox test(glm::vec3(1.0f), glm::vec3(1.0f));
	test.init();

	GLuint vao;
	GLuint PID = init1(&vao);

	glm::mat4 proj = glm::perspective(glm::radians(g_rotate_angle), (float)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 50.0f);
	g_view = glm::lookAt(glm::vec3(0.0f, 0.0f, g_cam_z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Light pose
	const float interpolateDuration = 2.0f;
	glm::vec3 startLightPosition(2.0f, 2.0, 2.0f);
	glm::vec3 endLightPosition(-2.0f, -2.0, 2.0f);
	float time = glfwGetTime();

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		//float elapsedTime = glfwGetTime() - time;
		//if (elapsedTime > interpolateDuration)
		//{
		//	glm::vec3 temp = endLightPosition;
		//	endLightPosition = startLightPosition;
		//	startLightPosition = temp;
		//	time = glfwGetTime();
		//}

		//float interpolate = elapsedTime / interpolateDuration;
		//glm::vec3 lightPosition = interpolate * (endLightPosition - startLightPosition) + startLightPosition;

		glm::mat4 modelView = g_view * g_model;
		glm::mat4 modelViewProj = proj * modelView;
		glm::mat3 modelViewNorm = glm::inverseTranspose(glm::mat3(modelView));

		glClearColor(0.0f, 0.4f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, gl_polygon_mode); // moved into render loop since interactive now.

		test.translateTo(endLightPosition, 10.0f);
		test.update(proj, g_view);

		// // dynamically located uniforms in shader program and pass the model/view/proj matrices
		glUniformMatrix4fv(glGetUniformLocation(PID, "mModelView"), 1, GL_FALSE, &modelView[0][0]); // get location of input shader variable and then pass the matrix
		glUniformMatrix4fv(glGetUniformLocation(PID, "mModelViewProj"), 1, GL_FALSE, &modelViewProj[0][0]); //
		glUniformMatrix3fv(glGetUniformLocation(PID, "mModelViewNorm"), 1, GL_FALSE, &modelViewNorm[0][0]); //
		glUniform3fv(glGetUniformLocation(PID, "vLightPosition"), 1, &test.translation()[0]);

		glBindVertexArray(vao); // since we only have a single VAO there's no need to (un)bind it every time, but we'll do so to establish a good habit

		for (int i = 0; i < NUM_TEXTURES; i++)
		{
			glUniform1f(glGetUniformLocation(PID, "fShininess"), g_materials[i].shininess);
			glUniform3fv(glGetUniformLocation(PID, "vAmbientColor"), 1, &g_materials[i].ambientColor[0]);
			glUniform3fv(glGetUniformLocation(PID, "vDiffuseColor"), 1, &g_materials[i].diffuseColor[0]);
			glUniform3fv(glGetUniformLocation(PID, "vSpecularColor"), 1, &g_materials[i].specularColor[0]);

			glBindTexture(GL_TEXTURE_2D, g_textures[i]);
			glDrawArrays(GL_TRIANGLES, 6 * i, 6); // draw the points
		}

		glBindVertexArray(0);
		glFlush();

		glfwSwapBuffers(window);
		glfwPollEvents(); // rprocess any input events (keyboard, mouse, etc)
	}
	glfwTerminate();
	return 0;
}
