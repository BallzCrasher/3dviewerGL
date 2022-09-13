#ifndef H_HELPER_FUNCTIONS
#define H_HELPER_FUNCTIONS
#include <cstdio>
#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <chrono>
#include <string>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include "model.h"
#include "Shader.h"
#include "stb_image.h"

#define WIDTH 800
#define HEIGHT 600

float lastMouseX = WIDTH / 2, lastMouseY = HEIGHT / 2;
const float mouse_sensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;
float cameraSpeed = 2.5f;
float deltaTime = 0.0f;
float fov = 45.0f;
bool firstMouseEnter = true;
glm::vec3 cameraPos   = glm::vec3(0.0f,0.0f,3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f,1.0f,0.0f);
bool flashLight_switch = false;


void framebuffer_size_callback(GLFWwindow* window, int width , int height){
    glViewport(0,0,width,height);
}

void scroll_callback(GLFWwindow* window , double xoffset , double yoffset){
	fov -= (float)yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 45.0f) fov = 45.0f;
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


void toggleFlashlight(){ 
	flashLight_switch = !flashLight_switch;
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

std::ostream& operator<<(std::ostream& stream, glm::mat4 mat){
	stream << std::fixed << std::setprecision(2);
	for(int i= 0; i < 4;i++){
		stream << "| "; 
		for(int j = 0;j < 4; j++) std::cout << mat[i][j] << ' '; 
		stream << " |\n";
	}
	stream << std::endl;
	return stream;
}

unsigned int load_texture(const char* texture_path){	
    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    int x,y,nrChannel;
    unsigned char* data = stbi_load(texture_path,&x,&y,&nrChannel,0);
    if (data){
		GLenum format;
			if (nrChannel == 1)
				format = GL_RED;
			else if (nrChannel == 3)
				format = GL_RGB;
			else if (nrChannel == 4)
				format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,x,y,0,format,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"FAILED TO LOAD IMAGE!"<<std::endl;
    }
    stbi_image_free(data);
	return texture;
}


glm::mat4 look_at(glm::vec3 position ,glm::vec3 target,glm::vec3 world_up){
	glm::vec3 zaxis = glm::normalize(position - target);
	glm::vec3 xaxis = glm::normalize(glm::cross(world_up, zaxis));
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	glm::mat4 left = glm::transpose(glm::mat4(
			glm::vec4(xaxis,0.0f),
			glm::vec4(yaxis,0.0f),
			glm::vec4(zaxis,0.0f),
			glm::vec4(0.0f,0.0f,0.0f,1.0f))
		);

	glm::mat4 right = glm::translate(glm::mat4(1.0f), -position);
	
	return left * right;
}

void setPointLight(unsigned int shaderID, size_t index 
		,glm::vec3 light_position
		,glm::vec3 light_ambient,glm::vec3 light_diffuse, glm::vec3 light_specular
		,float constant , float linear , float quadratic)
{ 
	std::string s_pos   = "pointLights[" + std::to_string(index) + "].position";
	std::string s_amb   = "pointLights[" + std::to_string(index) + "].ambient";
	std::string s_dif   = "pointLights[" + std::to_string(index) + "].diffuse";
	std::string s_spec  = "pointLights[" + std::to_string(index) + "].specular";
	std::string s_const = "pointLights[" + std::to_string(index) + "].constant";
	std::string s_lin   = "pointLights[" + std::to_string(index) + "].linear";
	std::string s_quad  = "pointLights[" + std::to_string(index) + "].quadratic";

	glUniform3fv(glGetUniformLocation(shaderID,s_pos .c_str()), 1, glm::value_ptr(light_position));
	glUniform3fv(glGetUniformLocation(shaderID,s_amb .c_str()), 1, glm::value_ptr(light_ambient));
	glUniform3fv(glGetUniformLocation(shaderID,s_dif .c_str()), 1, glm::value_ptr(light_diffuse));
	glUniform3fv(glGetUniformLocation(shaderID,s_spec.c_str()), 1, glm::value_ptr(light_specular));
	glUniform1f(glGetUniformLocation(shaderID,s_const.c_str()) ,constant);
	glUniform1f(glGetUniformLocation(shaderID,s_lin  .c_str()) ,linear);
	glUniform1f(glGetUniformLocation(shaderID,s_quad .c_str()) ,quadratic);
}
void setSpotLight(unsigned int shaderID
		,glm::vec3 light_position , glm::vec3 light_direction
		,glm::vec3 light_ambient  , glm::vec3 light_diffuse , glm::vec3 light_specular
		,float constant , float linear , float quadratic
		,float cutoff, float outerCutoff)
{
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.position"), 1, glm::value_ptr(light_position));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.direction"), 1, glm::value_ptr(light_direction));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.ambient"), 1, glm::value_ptr(light_ambient));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.diffuse"), 1, glm::value_ptr(light_diffuse));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.specular"), 1, glm::value_ptr(light_specular));
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.constant") ,constant);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.linear") ,linear);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.quadratic") ,quadratic);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.cutoff") ,cutoff);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.outerCutoff") ,outerCutoff);
}

void setDirectionalLight(unsigned int shaderID ,glm::vec3 light_direction
		,glm::vec3 light_ambient  , glm::vec3 light_diffuse , glm::vec3 light_specular)
{
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.direction"), 1, glm::value_ptr(light_direction));
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.ambient"), 1, glm::value_ptr(light_ambient));
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.diffuse"), 1, glm::value_ptr(light_diffuse));
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.specular"), 1, glm::value_ptr(light_specular));
}


void drawObject_outlined(const Model& object ,Shader& shader ,Shader& outline_shader,
				  glm::mat4 model, const glm::mat4& view, const glm::mat4& projection,const glm::vec3& scaleVector)
//color is controlled though outline_shader
{ 
		//mask the container
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
		glStencilFunc(GL_ALWAYS,1,0xFF);
		glStencilMask(0xFF);
		object.draw(shader);

		//draw only where the fragment isn't masked
		glStencilFunc(GL_NOTEQUAL,1,0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		//Draw scaled up container 
		outline_shader.use();
		model = glm::scale(model,scaleVector);
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"view"),1,GL_FALSE,glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"model"),1,GL_FALSE,glm::value_ptr(model));
		object.draw(outline_shader);
		
		//undo the stuff to draw other models
		//glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS,1,0xFF);
		glEnable(GL_DEPTH_TEST);

		glStencilMask(0x00);
}
#endif
