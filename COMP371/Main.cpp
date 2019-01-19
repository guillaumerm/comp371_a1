//example skeleton code 2019 winter comp371
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "OBJloader.h"  //include the object loader
#include "shaderloader.h"
using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;
GLFWwindow *window;

//Struct that contains all variables related to the camera.
struct {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, -3.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 euler_angles = glm::vec3(0.0f, -90.0f, 0.0f);
	float fov = 45.0f;

	glm::vec3 right() {
		return glm::normalize(glm::cross(this->front, this->up));
	}
} camera;

struct {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
} object;

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_W) {
		camera.position += camera.front;
		printf("[Log]Moving forward\n");
	}
	else if (key == GLFW_KEY_S) {
		camera.position -= camera.front;
		printf("[Log]Moving backwards\n");
	}
	else if (key == GLFW_KEY_A) {
		camera.position += camera.right();
		printf("[Log]Moving left\n");
	}
	else if (key == GLFW_KEY_D) {
		camera.position -= camera.right();
		printf("[Log]Moving right\n");
	}
	else if (key == GLFW_KEY_UP) {
		camera.euler_angles.x -= 1.0f;
		printf("[Log]Rotating up (angle = %f)\n", camera.euler_angles.x);
	}
	else if (key == GLFW_KEY_DOWN) {
		camera.euler_angles.x += 1.0f;
		printf("[Log]Rotating down (angle = %f)\n", camera.euler_angles.x);
	}
	else if (key == GLFW_KEY_RIGHT) {
		camera.euler_angles.y += 1.0f;
		printf("[Log]Rotating right (angle = %f)\n", camera.euler_angles.y);
	}
	else if (key == GLFW_KEY_LEFT) {
		camera.euler_angles.y -= 1.0f;
		printf("[Log]Rotating left (angle = %f)\n", camera.euler_angles.y);
	}
	else if (key == GLFW_KEY_J) {
		object.position.x += 1.0f;
		printf("[Log]Moving object on +X (X = %f)\n", object.position.x);
	}
	else if (key == GLFW_KEY_L) {
		object.position.x -= 1.0f;
		printf("[Log]Moving object on -X (X = %f)\n", object.position.x);
	}
	else if (key == GLFW_KEY_I) {
		object.position.y += 1.0f;
		printf("[Log]Moving object on +Y (X = %f)\n", object.position.y);
	}
	else if (key == GLFW_KEY_K) {
		object.position.y -= 1.0f;
		printf("[Log]Moving object on -Y (X = %f)\n", object.position.y);
	}
	else if (key == GLFW_KEY_PAGE_UP) {
		object.position.z += 1.0f;
		printf("[Log]Moving object on +Z (X = %f)\n", object.position.z);
	}
	else if (key == GLFW_KEY_PAGE_DOWN) {
		object.position.z -= 1.0f;
		printf("[Log]Moving object on -Z (X = %f)\n", object.position.z);
	}
	else if (key == GLFW_KEY_O) {
		object.scale.x = object.scale.y = object.scale.z *= 1.10f;
		printf("[Log]Scaling object by a factor of 10% (scale = %f\%)\n", object.scale.x);
	}
	else if (key == GLFW_KEY_P) {
		object.scale.x = object.scale.y = object.scale.z *= 0.9f;
		printf("[Log]Scaling object by a factor of -10% (scale = %f\%)\n", object.scale.x);
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//Keeps the previsou ypos of the cursor
	static double prev_ypos = -1;

	//Initialize the prev_ypos
	prev_ypos = (prev_ypos == -1) ? ypos : prev_ypos;

	//Move into/out of the scene only when GLFW_MOUSE_BUTTON_LEFT is press
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (prev_ypos - ypos > 0) {
			printf("[Log]Moving into the scene\n");
			camera.fov -= 0.1f;
		}
		else if (prev_ypos - ypos < 0) {
			printf("[Log]Moving out of the scene\n");
			camera.fov += 0.1f;
		} else{
			printf("[Log]Camera is stationary\n");
		}
	}
	
	prev_ypos = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int init() {
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//WINDOW
	window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 1", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Define the depth buffer and enable it
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
// The MAIN function, from here we start the application and run the game loop
int main()
{
	if (init() != 0)
		return EXIT_FAILURE;
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}



	// Build and compile our shader program
	// Vertex shader

	GLuint shader = loadSHADER("vertex.shader", "fragment.shader");
	glUseProgram(shader);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	loadOBJ("cat.obj", vertices, normals, UVs); //read the vertices from the cat.obj file

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	GLuint vertices_VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertices_VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		//Building a perspective view
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		camera.front.x = cos(glm::radians(camera.euler_angles.x)) * cos(glm::radians(camera.euler_angles.y));
		camera.front.y = cos(glm::radians(camera.euler_angles.x));
		camera.front.z = cos(glm::radians(camera.euler_angles.z)) * sin(glm::radians(camera.euler_angles.y));


		model = glm::translate(model, object.position);
		model = glm::scale(model, object.scale);

		view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

		projection = glm::perspective(glm::radians(camera.fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		int modelLoc = glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projectionLoc = glGetUniformLocation(shader, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, vertices.size());
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
