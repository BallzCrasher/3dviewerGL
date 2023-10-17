#include <algorithm>
#include <iostream>
#include <cmath>
#include <chrono>
#include <string>
#include <cmath>
#include <vector>
#include <memory>
#include <array>

#include "helper.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "callbacks.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class BasicCube {
	glm::mat4 m_model;
	glm::vec3 m_coords;

	static constexpr std::array cubeVertices = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right        
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left        
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		// Left face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left       
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right      
		// Bottom face          
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left  
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	};
	static constexpr std::array indicies = {
		// Back face
		0, 1, 3,
		3, 2, 0,
		// Front face 
		4, 7, 5,
		7, 4, 6,
		// Left face 
		11, 8, 10,
		8, 11, 9,
		// Right face 
		14, 15, 13,
		13, 12, 14,
		// Bottom face 
		19, 16, 18,
		16, 19, 17,
		// Top face
		22, 23, 21,
		21, 20, 22
	};
	unsigned int cubeVAO, cubeVBO, cubeEBO;
public:
	void shift(const glm::vec3& v) {
		m_model = glm::translate(m_model, v);
		m_coords += v;
	}

	void scale(const glm::vec3& v) {
		m_model = glm::scale(m_model, v);
	}

	void rotate(float theta, const glm::vec3& axis) {
		m_model = glm::rotate(m_model, glm::radians(theta), axis);
	}

	void move_to(const glm::vec3& coords) {
		shift(coords - m_coords);
	}

	glm::vec3 coordinates() { return m_coords; };

	BasicCube(const glm::vec3& coords) : m_model(1.0f), m_coords(coords) {
		glGenBuffers(1, &cubeVBO);
		glGenBuffers(1, &cubeEBO);
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		shift(coords);
	}
	BasicCube() : BasicCube({0.0f, 0.0f, 0.0f}) {}

	void draw(glm::mat4 projection, glm::mat4 view, unsigned int texture_id, Shader& shader) {
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		shader.use();
		glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(m_model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
	}
};

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH,HEIGHT,"3DVGL",NULL,NULL);
  if (window == nullptr) {
      std::cout<<"ERROR :: GLFWWindow not created !!"<<'\n';
      glfwTerminate();
      return -1;
  }

	// setting callbacks
  glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	setKeysCallbacks(window);

	// loading function pointers from GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "ERROR :: Failed to load glad !!" << std::endl;
    glfwTerminate();
		exit(EXIT_FAILURE);
  }

	/* OpenGL global state */
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glEnable(GL_STENCIL_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	stbi_set_flip_vertically_on_load(true);
	const unsigned int uvGrid = load_texture("extra/images/uvgrid.png");

	Shader container_shader(
    "src/shaders/model.vs",
    "src/shaders/model.fs"
	);
  
  Shader light_shader(
    "src/shaders/light.vs",
    "src/shaders/light.fs"
  );


  BasicCube cube;
  cube.shift({0.0f, 2.0f, 2.0f});
  Model container("extra/models/container/container.obj");

	const glm::vec3 worldColor(0.2f,0.2f,0.2f);
	float currentFrame {}, lastFrame {};
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);
		glm::mat4 projection, view;
		projection = glm::perspective(glm::radians(fov), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
    view = look_at(cameraPos,cameraPos + cameraFront,cameraUp);
		//Calculate Delta Time
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		/* logic */
    glClearColor(worldColor.x, worldColor.y, worldColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    container_shader.use();
    glm::mat4 model(1.0f);
    container_shader.setVec3Uniform("viewPos", cameraPos);
    glUniformMatrix4fv(glGetUniformLocation(container_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(container_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(container_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    setPointLight(container_shader.ID, 0, cube.coordinates(),
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        1.0f, 0.09f, 0.032f
    );
    setDirectionalLight(container_shader.ID,
        {-1.0f, -1.0f, -1.0f},
        {0.001f, 0.001f, 0.001f},
        {0.002f, 0.002f, 0.001f},
        {0.005f, 0.005f, 0.005f}
    );

    glUniform1f(glGetUniformLocation(container_shader.ID, "material.shininess"), 1.0f);
    glUniform1i(glGetUniformLocation(container_shader.ID, "flashLightON"), flashLight_switch);
    setSpotLight(container_shader.ID,
        cameraPos, cameraFront,
        {1.0f, 1.0f, 1.0f},
        {1.0f, 0.5f, 0.3f},
        {0.5f, 0.5f, 0.5f},
        1.0f, 0.09, 0.032f,
        glm::cos(glm::radians(12.5f)),
        glm::cos(glm::radians(17.5f))
    );

    container.draw(container_shader);

    light_shader.use();
    light_shader.setVec3Uniform("lightColor", {1, 1, 1});
    cube.draw(projection, view, uvGrid, light_shader);

		/* end logic */
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0; 
}
