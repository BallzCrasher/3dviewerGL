#include <glad/glad.h>
#include <glm/geometric.hpp>
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
#include "includes/helper_functions.h"
#include "includes/model.h"
#include "includes/Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

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


int main(){ 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH,HEIGHT,"OpenGL Window",NULL,NULL);
    if (window == NULL){
        std::cout<<"ERROR :: GLFWWindow not created !!"<<'\n';
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

	glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout<<"ERROR :: Failed to load glad !!"<<std::endl;
        glfwTerminate();
        return -2;
    }


	//OpenGL global state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	stbi_set_flip_vertically_on_load(true);
	

	Shader shader("./shaders/vertexShader.glsl","./shaders/fragmentShader.glsl");
	Shader light_shader("./shaders/light_vertexShader.glsl","./shaders/light_fragmentShader.glsl");
	Shader outline_shader("./shaders/light_vertexShader.glsl","./shaders/outline_fragmentShader.glsl");

	std::cout << "LOADING MODELS" << std::endl;
	Model container("./models/container2/container2.obj");
	Model light_cube("./models/cube/shit.obj");
	Model plane("./models/plane/plane.obj");
	std::cout << "FINISHED LOADING MODELS" << std::endl;

	const glm::vec3 worldColor(0.2f,0.2f,0.2f);
	float currentFrame,lastFrame = 0.0f;
	glm::vec3 light_pos;
	glm::mat4 model, projection , view;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(worldColor.x,worldColor.y,worldColor.z,1.0f);
		glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);

		//Calculate Delta Time
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//draw floor
		light_shader.use();
		model = glm::translate(glm::mat4(1.0f) , glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));	
		glUniformMatrix4fv(glGetUniformLocation(light_shader.ID,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(light_shader.ID,"view"),1,GL_FALSE,glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(light_shader.ID,"model"),1,GL_FALSE,glm::value_ptr(model));
		light_shader.setVec3Uniform("lightColor", glm::vec3(0.5f));
		plane.draw(light_shader);
		
		light_pos = glm::vec3(2.0f * sinf(glfwGetTime()) ,2.0f,2.0f * cosf(glfwGetTime()) );

		//draw container
		shader.use();
		projection = glm::perspective(glm::radians(fov), (float) WIDTH /  (float) HEIGHT, 0.1f, 100.0f);
        view = look_at(cameraPos,cameraPos + cameraFront,cameraUp);
	    model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		glUniformMatrix4fv(glGetUniformLocation(shader.ID,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID,"view"),1,GL_FALSE,glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID,"model"),1,GL_FALSE,glm::value_ptr(model));
		shader.setVec3Uniform("viewPos",cameraPos.x , cameraPos.y , cameraPos.z);
		glUniform1i(glGetUniformLocation(shader.ID,"flashLightON"),flashLight_switch);
		glUniform1f(glGetUniformLocation(shader.ID,"material.shininess"),8.0f);

		setDirectionalLight(shader.ID, 
			 glm::vec3(-0.2f, -1.0f, -0.3f), // direction
			 glm::vec3(0.05f, 0.05f, 0.05f), // ambient
			 glm::vec3(0.4f, 0.4f, 0.4f), // diffuse
			 glm::vec3(0.5f, 0.5f, 0.5f) // specular
		);

		setPointLight(shader.ID, 0, light_pos	
				,glm::vec3(0.5f, 0.5f, 0.5f)       //light_ambient
				,glm::vec3(0.8f , 0.8f , 0.8f )    //light_diffuse
				,glm::vec3(1.0f , 1.0f , 1.0f )    //light_specular
				,1.0f, 0.9f, 0.32f
		);

		setSpotLight(shader.ID, cameraPos, cameraFront
				,glm::vec3(0.05f, 0.05f, 0.05f)    //light_ambient
				,glm::vec3(0.8f , 0.8f , 0.8f )    //light_diffuse
				,glm::vec3(1.0f , 1.0f , 1.0f )    //light_specular
				,1.0f, 0.9f, 0.32f
				,glm::cos(glm::radians(12.5f))
				,glm::cos(glm::radians(15.0f))
		);

		//drawing container with outline
		//mask the container
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
		glStencilFunc(GL_ALWAYS,1,0xFF);
		glStencilMask(0xFF);
		container.draw(shader);

		//draw only where the fragment isn't masked
		glStencilFunc(GL_NOTEQUAL,1,0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		//Draw scaled up container 
		outline_shader.use();
		model = glm::scale(model,glm::vec3(1.05f));
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"view"),1,GL_FALSE,glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"model"),1,GL_FALSE,glm::value_ptr(model));
		container.draw(outline_shader);
		
		//undo the stuff to draw other models
		//glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS,1,0xFF);
		glEnable(GL_DEPTH_TEST);

		glStencilMask(0x00);
		//draw light_cube
		light_shader.use();
		model = glm::translate(glm::mat4(1.0f) , light_pos);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	
		glUniformMatrix4fv(glGetUniformLocation(light_shader.ID,"model"),1,GL_FALSE,glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(light_shader.ID,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(light_shader.ID,"view"),1,GL_FALSE,glm::value_ptr(view));
		light_shader.setVec3Uniform("lightColor", glm::vec3(1.0f));
		light_cube.draw(light_shader);

	
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0; 
}
