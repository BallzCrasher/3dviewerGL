#include <glad/glad.h>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <chrono>
#include <string>
#include "includes/glm/ext/matrix_float3x3.hpp"
#include "includes/glm/ext/matrix_float4x4.hpp"
#include "includes/glm/ext/matrix_transform.hpp"
#include "includes/glm/ext/quaternion_geometric.hpp"
#include "includes/glm/ext/vector_float3.hpp"
#include "includes/glm/fwd.hpp"
#include "includes/glm/geometric.hpp"
#include "includes/glm/glm.hpp"
#include "includes/glm/gtc/matrix_transform.hpp"
#include "includes/glm/gtc/type_ptr.hpp"
#include "includes/Shader.h"
#include "includes/glm/matrix.hpp"
#include "includes/glm/trigonometric.hpp"
#include "includes/model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

#define WIDTH 800
#define HIEGHT 600

float lastMouseX = WIDTH / 2, lastMouseY = HIEGHT / 2;
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
bool flashLight_switch = true;

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


void processInput(GLFWwindow* window){
    if (glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
	static const glm::mat3 transform = glm::mat3(
			1.0f, 0.0f ,0.0f,
			0.0f, 0.0f ,0.0f,
			0.0f, 0.0f ,1.0f
		);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += deltaTime * cameraSpeed * (transform * cameraFront);	
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= deltaTime * cameraSpeed * (transform * cameraFront);	
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross((transform * cameraFront), cameraUp)) * cameraSpeed * deltaTime;	
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross((transform * cameraFront), cameraUp)) * cameraSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos.y += cameraSpeed * deltaTime;	
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos.y -= cameraSpeed * deltaTime;	

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		flashLight_switch = !flashLight_switch;

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
		, glm::vec3 light_position, glm::vec3 light_ambient
		, glm::vec3 light_diffuse, glm::vec3 light_specular
		, float constant , float linear , float quadratic)
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

int main(){ 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH,HIEGHT,"OpenGL Window",NULL,NULL);
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

    //shaders
    Shader shader("shaders/vertexShader.glsl","shaders/fragmentShader.glsl");
	Shader lightShader("shaders/light_vertexShader.glsl","shaders/light_fragmentShader.glsl");

    //TEXTURES 
	
	stbi_set_flip_vertically_on_load(true);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	unsigned int diffuseMap = load_texture("./images/container2.png");
	unsigned int specularMap = load_texture("./images/container2_specular.png");

    //making buffers
	float verts[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

    unsigned int VBO,VAO,lightVAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);

    //vertex attrib pointers .....
    
	//position
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

	//normal
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3 * sizeof(float)) );
    glEnableVertexAttribArray(1);

	//texture coords	
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6 * sizeof(float)) );
    glEnableVertexAttribArray(2);

	//light cube pointers .....
    glGenVertexArrays(1,&lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

	glVertexAttribPointer(0 , 3 ,GL_FLOAT , GL_FALSE , 8 * sizeof(float) , (void *) 0);
    glEnableVertexAttribArray(0);


	//glm::vec3 cubePos(0.0f , 0.0f ,0.0f);
	//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	
	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};  


    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	const glm::vec3 worldColor(0.75f,0.5f,0.1f);
	float currentFrame,lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(worldColor.x,worldColor.y,worldColor.z,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		//Calculate Delta Time
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//lightPos = glm::vec3( 3 * cosf(glfwGetTime()) , lightPos.y , 3 * sinf(glfwGetTime()) );

		glm::vec3 lightColor = glm::vec3(1.0f);

		shader.use();
		shader.setVec3Uniform("viewPos", cameraPos.x, cameraPos.y, cameraPos.z); 

		//flashlight
		shader.setVec3Uniform("spotLight.position", cameraPos.x, cameraPos.y, cameraPos.z); 
		shader.setVec3Uniform("spotLight.direction", cameraFront.x, cameraFront.y, cameraFront.z);
		glUniform1f(glGetUniformLocation(shader.ID,"spotLight.cutoff"),glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(shader.ID,"spotLight.outerCutoff"),glm::cos(glm::radians(17.5f)));	 
		glUniform1f(glGetUniformLocation(shader.ID,"spotLight.constant") ,1.0f);
		glUniform1f(glGetUniformLocation(shader.ID,"spotLight.linear")   ,0.09f);
		glUniform1f(glGetUniformLocation(shader.ID,"spotLight.quadratic"),0.0032f);
		if (flashLight_switch){ 
			shader.setVec3Uniform("spotLight.ambient", 0.2f, 0.2f, 0.2f); //flashLight propreties
			shader.setVec3Uniform("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
			shader.setVec3Uniform("spotLight.specular", 1.0f, 1.0f, 1.0f);
		} else { 
			shader.setVec3Uniform("spotLight.ambient" ,glm::vec3(0.0f) ); 
			shader.setVec3Uniform("spotLight.diffuse" ,glm::vec3(0.0f) );
			shader.setVec3Uniform("spotLight.specular",glm::vec3(0.0f) );
		}

		//Directional Light
		shader.setVec3Uniform("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.setVec3Uniform("dirLight.ambient", worldColor);
        shader.setVec3Uniform("dirLight.diffuse", worldColor);
        shader.setVec3Uniform("dirLight.specular", worldColor);

		//Point Lights	
		for(int i =0 ; i < 4 ; i++) { 
			setPointLight(shader.ID,i
					,pointLightPositions[i]      // position
					,glm::vec3(0.05, 0.05, 0.05) // ambient
					,glm::vec3(0.5f, 0.5f, 0.5f) // diffuse
					,glm::vec3(1.0f, 1.0f, 1.0f) // specular
					,1.0f                        // constant
					,0.09f                       // linear
					,0.032f                      // quadratic
				);
		}

		//material propreties
		glUniform1i(glGetUniformLocation(shader.ID,"material.diffuse") ,0);
		glUniform1i(glGetUniformLocation(shader.ID,"material.specular"),1);
		glUniform1f(glGetUniformLocation(shader.ID,"material.shininess"),32.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glm::mat4 view = look_at(cameraPos,cameraPos + cameraFront , cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO);
		for(unsigned int i = 0; i < 10; i++){
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lightShader.use();
        
		lightShader.setVec3Uniform("lightColor",  lightColor.x, lightColor.y, lightColor.z);
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		for(unsigned int i = 0; i < 4; i++){
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); 

			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
         
        glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES,0,36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0; 
}
