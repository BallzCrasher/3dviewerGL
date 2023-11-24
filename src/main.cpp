#include <algorithm>
#include <iostream>
#include <cmath>
#include <chrono>
#include <string>
#include <cmath>
#include <vector>
#include <memory>
#include <array>

#include "helper_objects.hpp"
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

int main() {
	// initializing glfw with GL version 3.3
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// creating a window
	GLFWwindow* window = glfwCreateWindow(GlobalVars::WIDTH, GlobalVars::HEIGHT, "3DVGL", nullptr, nullptr);
  if (window == nullptr) {
      std::cout<<"ERROR :: GLFWWindow not created !!"<<'\n';
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(window);

	// setting callbacks
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

	// OpenGL global state
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	stbi_set_flip_vertically_on_load(true);
	//load default grid
	const unsigned int uvGrid {load_texture("extra/images/uvgrid.png")};

	// compiling shaders
	Shader container_shader {
    "src/shaders/model.vs",
    "src/shaders/model.fs"
	};
  Shader light_shader {
    "src/shaders/light.vs",
    "src/shaders/light.fs"
	};

  Shader outline_shader {
    "src/shaders/default.vs",
    "src/shaders/outline.fs"
	};


	// loading models
  BasicCube cube;
  cube.shift({0.0f, 2.0f, 2.0f});
  Model container {"extra/models/container/container.obj"}; 

	// setting world color
	const glm::vec3 worldColor {0.2f, 0.2f, 0.2f};
  glClearColor(worldColor.x, worldColor.y, worldColor.z, 1.0f);

	float current_frame {}, last_frame {};
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);
		glm::mat4 projection, view;
		projection = glm::perspective(
			glm::radians(GlobalVars::fov), 
			static_cast<float>(GlobalVars::WIDTH) / GlobalVars::HEIGHT,
			0.1f,
			100.0f
		);
    view = look_at(
			GlobalVars::camera_pos, 
			GlobalVars::camera_pos + GlobalVars::camera_front,
			GlobalVars::camera_up
		);
		// calculating delta time
		current_frame = glfwGetTime();
		GlobalVars::delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// -- loop logic --
		// clearing buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
		//disable writing to the Stencil Buffer
		glStencilMask(0x00);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		// initializing container shader
		container_shader.use();
    glm::mat4 model(1.0f);
    container_shader.setVec3  ("viewPos",      GlobalVars::camera_pos);
    container_shader.setMatrix("model",        model);
    container_shader.setMatrix("view",         view);
    container_shader.setMatrix("projection",   projection);
    set_point_light(container_shader, 
				0, 
				cube.coordinates(),
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        1.0f, 0.09f, 0.032f
    );
    set_directional_light(container_shader,
        {-1.0f, -1.0f, -1.0f},
        {0.001f, 0.001f, 0.001f},
        {0.002f, 0.002f, 0.001f},
        {0.005f, 0.005f, 0.005f}
    );
    container_shader.setFloat("material.shininess", 1.0f);
    container_shader.setInt("flashLightON", GlobalVars::flashlight_switch);
    set_spot_light(container_shader,
        GlobalVars::camera_pos, GlobalVars::camera_front,
        {1.0f, 1.0f, 1.0f},
        {1.0f, 0.5f, 0.3f},
        {0.5f, 0.5f, 0.5f},
        1.0f, 0.09, 0.032f,
        glm::radians(12.5f),
        glm::radians(17.5f)
    );
		// drawing container
    //container.draw(container_shader);
		draw_outlined(container, 
			container_shader, outline_shader,
			model, view, projection, 
			{1.1f, 1.1f, 1.1f}
		);

		// initializing light shader
    light_shader.use();
    light_shader.setVec3("lightColor", {1.0f, 1.0f, 1.0f});
		// drawing cube
    cube.draw(projection, view, uvGrid, light_shader);

		/* end logic */
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0; 
}
