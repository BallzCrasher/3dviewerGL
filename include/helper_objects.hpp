#ifndef H_HELPER_OBJECTS
#define H_HELPER_OBJECTS
#include <array>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "shader.hpp"

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
	void shift(const glm::vec3& v);

	void scale(const glm::vec3& v);

	void rotate(float theta, const glm::vec3& axis);

	void move_to(const glm::vec3& coords);

	glm::vec3 coordinates();

	BasicCube(const glm::vec3& coords);
	BasicCube();

	void draw(glm::mat4 projection, glm::mat4 view, unsigned int texture_id, Shader& shader);
};
#endif

