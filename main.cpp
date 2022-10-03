#include <algorithm>
#include <glad/glad.h>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
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

	float planeVertices[] = {
		// positions             // texture Coords 
		0.0f,  0.5f,  0.0f,      0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,      0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,      1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,      0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,      1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,      1.0f,  0.0f
	};
	// plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	//OpenGL global state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	unsigned int window_image = load_texture("./images/blending_transparent_window.png");

	Shader light_shader("./shaders/light_vertexShader.glsl","./shaders/light_fragmentShader.glsl");
	Shader transparent_shader("./shaders/grass_vertexShader.glsl","./shaders/grass_fragmentShader.glsl");

	std::cout << "LOADING MODELS" << std::endl;
	Model plane("./models/plane/plane.obj");
	std::cout << "FINISHED LOADING MODELS" << std::endl;

	std::vector<glm::vec3> windows =
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3( 1.5f, 0.0f, 0.51f),
        glm::vec3( 0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3( 0.5f, 0.0f, -0.6f)
    };

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

		light_pos = glm::vec3(2.0f * sinf(glfwGetTime()) ,2.0f,2.0f * cosf(glfwGetTime()) );
		projection = glm::perspective(glm::radians(fov), (float) WIDTH /  (float) HEIGHT, 0.1f, 100.0f);
        view = look_at(cameraPos,cameraPos + cameraFront,cameraUp);

		std::sort(windows.begin(), windows.end(), [](const glm::vec3& a, const glm::vec3& b){ 
				return glm::distance(cameraPos , a) > glm::distance(cameraPos, b);
		});
		
		for(int i = 0; i < windows.size(); i++) { 
			//draw transparent
			transparent_shader.use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, window_image);
			glUniform1i(glGetUniformLocation(transparent_shader.ID,"grassTexture"), 0);
			model = glm::mat4(1.0f);
			model = glm::translate(model, windows[i]);
			glUniformMatrix4fv(glGetUniformLocation(transparent_shader.ID,"view"),1,GL_FALSE,glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(transparent_shader.ID,"projection"),1,GL_FALSE,glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(transparent_shader.ID,"model"),1,GL_FALSE,glm::value_ptr(model));
			glBindVertexArray(planeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0; 
}
