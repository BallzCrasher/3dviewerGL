#include <algorithm>
#include <glad/glad.h>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
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


int main(){ 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH,HEIGHT,"OpenGL Window",NULL,NULL);
    if (window == NULL) {
        std::cout<<"ERROR :: GLFWWindow not created !!"<<'\n';
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

	glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	setKeysCallbacks(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout<<"ERROR :: Failed to load glad !!"<<std::endl;
        glfwTerminate();
        return -2;
    }
	const float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left                
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right        
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left        
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left       
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right      
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		// Bottom face          
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left  
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left              
	};
	unsigned int cubeVAO, cubeVBO;
	glGenBuffers(1, &cubeVBO);
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	

	/* OpenGL global state */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	//glEnable(GL_STENCIL_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//unsigned int window_image = load_texture("./images/blending_transparent_window.png");
	const unsigned int uvGrid = load_texture("./images/uvgrid.png");

	Shader normal_shader(
			"./shaders/normal_vertexShader.glsl",
		    "./shaders/normal_fragmentShader.glsl"
	);

	const glm::vec3 worldColor(0.2f,0.2f,0.2f);
	float currentFrame,lastFrame = 0.0f;
	glm::mat4 model, projection , view;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(worldColor.x,worldColor.y,worldColor.z,1.0f);
		//glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glStencilMask(0x00);
		projection = glm::perspective(glm::radians(fov), (float) WIDTH /  (float) HEIGHT, 0.1f, 100.0f);
        view = look_at(cameraPos,cameraPos + cameraFront,cameraUp);

		//Calculate Delta Time
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/* logic */

		{  // draw cube
			glBindVertexArray(cubeVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, uvGrid);
			normal_shader.use();
			glUniform1i(glGetUniformLocation(normal_shader.ID, "texture1"), 0);
			model = glm::mat4(1.0f);
			glUniformMatrix4fv(glGetUniformLocation(normal_shader.ID,"view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(normal_shader.ID,"model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(normal_shader.ID,"projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/* end logic */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0; 
}
