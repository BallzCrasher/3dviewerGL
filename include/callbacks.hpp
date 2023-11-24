#ifndef CALLBACKS_H
#define CALLBACKS_H
#include <cmath>
#include <iostream>

#include "callbacks.hpp"
#include "variables.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width , int height){
    glViewport(0, 0, width, height);
}

void scroll_callback([[maybe_unused]]GLFWwindow* window ,[[maybe_unused]] double xoffset , double yoffset){
	GlobalVars::fov -= (float)yoffset;
	if (GlobalVars::fov < 1.0f) GlobalVars::fov = 1.0f;
	if (GlobalVars::fov > 45.0f) GlobalVars::fov = 45.0f;
}

void setKeysCallbacks(GLFWwindow* window){ 
	auto flashLight_callback = []
		([[maybe_unused]] GLFWwindow* winow, int key, [[maybe_unused]] int scancode, int action,[[maybe_unused]] int mods)
	{ 
		if (key == GLFW_KEY_F && action == GLFW_PRESS) { 
			GlobalVars::flashlight_switch = !GlobalVars::flashlight_switch;
		}
	};
	glfwSetKeyCallback(window, flashLight_callback);
}

void mouse_callback([[maybe_unused]] GLFWwindow* window, double xpos , double ypos){
	if(GlobalVars::first_mouse_enter){
		GlobalVars::last_mouse_x = xpos;	
		GlobalVars::last_mouse_y = ypos;	
		GlobalVars::first_mouse_enter = false;
	}

	float xoffset = xpos - GlobalVars::last_mouse_x;	
	float yoffset = GlobalVars::last_mouse_y - ypos;

	GlobalVars::last_mouse_x = xpos;
	GlobalVars::last_mouse_y = ypos;

	xoffset *= GlobalVars::MOUSE_SENSITIVITY;
	yoffset *= GlobalVars::MOUSE_SENSITIVITY;

	GlobalVars::yaw   += xoffset;
	GlobalVars::pitch += yoffset;

	if(GlobalVars::pitch > 89.0f)  GlobalVars::pitch =  89.0f;
	if(GlobalVars::pitch < -89.0f) GlobalVars::pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(GlobalVars::yaw)) * cos(glm::radians(GlobalVars::pitch));
	direction.y = sin(glm::radians(GlobalVars::pitch));
	direction.z = sin(glm::radians(GlobalVars::yaw)) * cos(glm::radians(GlobalVars::pitch));
	GlobalVars::camera_front = glm::normalize(direction);
}


void processInput(GLFWwindow* window) {
	using namespace GlobalVars;
	if (glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
    
	glm::vec3 move_dir = glm::normalize(glm::vec3(camera_front.x, 0.0f, camera_front.z));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera_pos += delta_time * camera_speed * move_dir;	
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera_pos -= delta_time * camera_speed * move_dir;	
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera_pos -= glm::normalize(glm::cross(move_dir, camera_up)) * camera_speed * delta_time;	
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera_pos += glm::normalize(glm::cross(move_dir, camera_up)) * camera_speed * delta_time;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera_pos.y += camera_speed * delta_time;	
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera_pos.y -= camera_speed * delta_time;	
}

#endif
