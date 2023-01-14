#ifndef CALLBACKS_H
#define CALLBACKS_H
#include <GLFW/glfw3.h>
#include <variables.hpp>
#include <cmath>
#include <glm/glm.hpp>
void framebuffer_size_callback(GLFWwindow* window, int width , int height){
    glViewport(0,0,width,height);
}

void scroll_callback(GLFWwindow* window , double xoffset , double yoffset){
	fov -= (float)yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 45.0f) fov = 45.0f;
}

void setKeysCallbacks(GLFWwindow* window){ 
	auto flashLight_callback = []
		(GLFWwindow* winow, int key, int scancode, int action, int mods)
	{ 
		if (key == GLFW_KEY_F && action == GLFW_PRESS) { 
			flashLight_switch = !flashLight_switch;
		}
	};
	glfwSetKeyCallback(window, flashLight_callback);
}

void mouse_callback(GLFWwindow* window, double xpos , double ypos){
	if(firstMouseEnter){
		lastMouseX = xpos;	
		lastMouseY = ypos;	
		firstMouseEnter = false;
	}

	float xoffset = xpos - lastMouseX;	
	float yoffset = lastMouseY - ypos;

	lastMouseX = xpos;
	lastMouseY = ypos;

	xoffset *= mouse_sensitivity;
	yoffset *= mouse_sensitivity;

	yaw   += xoffset;
	pitch += yoffset;

	if(pitch > 89.0f) pitch = 89.0f;
	if(pitch < -89.0f) pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}


void processInput(GLFWwindow* window){
    if (glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
	static const glm::mat3 transform = glm::mat3(
			1.0f, 0.0f ,0.0f,
			0.0f, 0.0f ,0.0f,
			0.0f, 0.0f ,1.0f
		);

	glm::vec3 moveDir = glm::normalize(glm::vec3(cameraFront.x,0.0f,cameraFront.z));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += deltaTime * cameraSpeed * moveDir;	
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= deltaTime * cameraSpeed * moveDir;	
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(moveDir, cameraUp)) * cameraSpeed * deltaTime;	
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(moveDir, cameraUp)) * cameraSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos.y += cameraSpeed * deltaTime;	
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos.y -= cameraSpeed * deltaTime;	
}
#endif
