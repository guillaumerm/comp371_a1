//example skeleton code 2019 winter comp371
//modified from http://learnopengl.com/

#include <GL/glew.h>	// include GL Extension Wrangler
#include <glfw/glfw3.h>	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OBJloader.h"  //include the object loadr
#include "shaderloader.h"
using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;
GLFWwindow *window;

//Struct that contains all variables and behaviors related to the camera.
struct {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 55.0f); /*!< Position of the camera */
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f); /*!< Front of the camera */
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); /*!< Up of the camera */
	float fov = 45.0f; /*!< FOV of the camera */

    //! Caculates the right vector. That is the result of the cross product between FRONT and and UP vectors. 
	glm::vec3 right() {
		return glm::normalize(glm::cross(this->front, this->up));
	}

    //! Pitch (rotate upon the RIGHT vector) the camera at a certain angle from its current orientation.
    /*!
      \param angle Angle, expressed in degrees, at which the camera will be pitched.
    */
	void pitch(float angle) {
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), this->right());
		this->front = glm::vec3(glm::vec4(this->front, 0) * rotation);
	}

    //! Yaw (rotate upon the UP vector) the camera at a certain angle from its current orientation
    /*!
      \param angle Angle, expressed in degrees, at which the camera will be yawed.
    */
	void yaw(float angle) {
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), this->up);
		this->front = glm::vec3(glm::vec4(this->front, 0) * rotation);
	}

    //! Reset the camera to its initial configurations.
	void reset() {
		position = glm::vec3(0.0f, 0.0f, 55.0f);
		front = glm::vec3(0.0f, 0.0f, -1.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		fov = 45.0f;
	}
} camera;

//Struct that contains all variables and behaviors related to the object
struct {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); /*!< Position of the object */
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f); /*!< Current scale of the object */
	float pitch = 0.0f; /*!< Current pitch of the object */
	float yaw = 0.0f; /*!< Current yuw of the object */
	float roll = 0.0f; /*!< Current roll of the object */

	//! Reset the object to its initial configurations.
	void reset() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
		pitch = 0.0f;
		yaw = 0.0f;
		roll = 0.0f;
	}
} object;

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// If the key is being release ignore it
	if(action == GLFW_RELEASE){
		return;
	}

	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_R) {
		camera.reset();
		object.reset();
	}
	else if (key == GLFW_KEY_W) {
		camera.position += camera.front;
	}
	else if (key == GLFW_KEY_S) {
		camera.position -= camera.front;
	}
	else if (key == GLFW_KEY_A) {
		camera.position -= camera.right();
	}
	else if (key == GLFW_KEY_D) {
		camera.position += camera.right();
	}
	else if (key == GLFW_KEY_UP) {
		camera.pitch(-5.0f);
	}
	else if (key == GLFW_KEY_DOWN) {
		camera.pitch(5.0f);
	}
	else if (key == GLFW_KEY_RIGHT) {
		camera.yaw(5.0f);
	}
	else if (key == GLFW_KEY_LEFT) {
		camera.yaw(-5.0f);
	}
	else if (key == GLFW_KEY_J) {
		object.position.x += 1.0f;
	}
	else if (key == GLFW_KEY_L) {
		object.position.x -= 1.0f;
	}
	else if (key == GLFW_KEY_I) {
		object.position.y += 1.0f;
	}
	else if (key == GLFW_KEY_K) {
		object.position.y -= 1.0f;
	}
	else if (key == GLFW_KEY_PAGE_UP) {
		object.position.z += 1.0f;
	}
	else if (key == GLFW_KEY_PAGE_DOWN) {
		object.position.z -= 1.0f;
	}
	else if (key == GLFW_KEY_O) {
		object.scale.x = object.scale.y = object.scale.z *= 1.10f;
	}
	else if (key == GLFW_KEY_P) {
		object.scale.x = object.scale.y = object.scale.z *= 0.9f;
	}
	else if (key == GLFW_KEY_B) {
		if(object.pitch + 1.0f > 360.0f){
			object.pitch = 0.0f;
		} else {
			object.pitch += 1.0f;
		}
	}
	else if (key == GLFW_KEY_N) {
		if(object.yaw + 1.0f > 360.0f){
			object.yaw = 0.0f;
		} else {
			object.yaw += 1.0f;
		}
	}
	else if (key == GLFW_KEY_E) {
		if(object.roll + 1.0f > 360.0f){
			object.roll = 0.0f;
		} else {
			object.roll += 1.0f;
		}
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//Keeps the previsou ypos of the cursor
	static double prev_ypos = -1;

	//Initialize the prev_ypos if first cursor_position_callback
	if(prev_ypos == -1)
		prev_ypos = ypos;

	//Move into/out of the scene (assumed that means chaning fov) only when GLFW_MOUSE_BUTTON_LEFT is press
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		double ypos_delta = prev_ypos - ypos;
		// If ypos_delta < 0 then zooming in if ypos_delta > 0 zooming out
		if(ypos_delta < 0) {
			camera.fov += 0.1f;
		} else if (ypos_delta > 0) {
			camera.fov -= 0.1f;
		}
	}
	
	// Keeping track of ypos
	prev_ypos = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int init() {
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

	GLuint shader = loadSHADER("./shaders/vertex.shader", "./shaders/fragment.shader");
	glUseProgram(shader);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	loadOBJ("./objects/cat.obj", vertices, normals, UVs); //read the vertices from the cat.obj file

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
		// Clear the colorbuffer and depth buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		//Building a perspective view
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);


		
		// Constructing the ctm (camera transformation matrix)
		
		model = glm::scale(model, object.scale);
		// Construct the model_rotation matrix
		glm::mat4 model_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(object.roll), glm::vec3(0,0,1))*glm::rotate(glm::mat4(1.0f), glm::radians(object.yaw), glm::vec3(0, 1, 0))* glm::rotate(glm::mat4(1.0f), glm::radians(object.pitch), glm::vec3(1,0,0));
		model *= model_rotation;
		model = glm::translate(model, object.position);

		view = glm::lookAt(camera.position, camera.front + camera.position, camera.up);

		projection = glm::perspective(glm::radians(camera.fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		glm::mat4 ctm = projection * view * model;

		//Passing the ctm (camera transformation matrix) to the shaders
		int ctmLocation = glGetUniformLocation(shader, "camera_transformation_matrix");
		glUniformMatrix4fv(ctmLocation, 1, GL_FALSE, glm::value_ptr(ctm));

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
