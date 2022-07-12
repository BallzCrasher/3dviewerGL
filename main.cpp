#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <chrono>
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
#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

#define WIDTH 800
#define HIEGHT 600

float lastMouseX = WIDTH / 2, lastMouseY = HIEGHT / 2;
const float mouse_sensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;
float changer = 0.01f;
float cameraSpeed = 2.5f;
float deltaTime = 0.0f;
float fov = 45.0f;
bool firstMouseEnter = true;
glm::vec3 cameraPos   = glm::vec3(0.0f,0.0f,3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f,1.0f,0.0f);

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
    
	glm::mat3 transform = glm::mat3(
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
	

    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
        changer += 0.01f;
        if (changer > 0.99) changer -= 0.01f;
    }

    if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
        changer -= 0.01f;
        if (changer < 0.01) changer += 0.01f;
    }
}




std::ostream& operator<<(std::ostream& stream, glm::mat4 mat){
	stream << std::fixed << std::setprecision(2);
	auto p = glm::value_ptr(mat);
	for(int j = 0;j < 4; j++){
		stream << "| "; 
		for(int i= 0; i < 4;i++) std::cout << *(p+4*j+i) << ' '; 
		stream << " |\n";
	}
	stream << std::endl;
	return stream;
}


void CreateCircle(float verts[],int indicies[], int resolution, float radius){
    float ang = 0;
    float incr = (2*M_PI)/resolution;
    verts[0] = verts[1] = verts [2] = 0.0f;
    for (int i=1;i<resolution+1;i++){
        verts[3*i] = radius*cosf(ang); if (abs(verts[3*i]) < 0.00001f) verts[3*i] = 0.0f; //rounding errors
        verts[3*i+1] = radius*sinf(ang); if (abs(verts[3*i+1]) < 0.00001f) verts[3*i+1] = 0.0f;
        verts[3*i+2] = 0.0f;
        ang+=incr;
    }
    for(int i=0;i<resolution-1;i++){
        indicies[3*i] = 0;
        indicies[3*i+1] = i+1;
        indicies[3*i+2] = i+2;
    }

    indicies[3*(resolution-1)] = 0;
    indicies[3*(resolution-1)+1] = resolution;
    indicies[3*(resolution-1)+2] = 1;
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

int main(){ 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH,HIEGHT,"OpenGL Window",NULL,NULL);
    if (window == NULL){
        std::cout<<"ERROR :: GLFWWindow not created !! "<<'\n';
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout<<"ERROR :: Failed to load glad !!"<<std::endl;
        glfwTerminate();
        return -2;
    }

	glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
    //making shader
    Shader shader("shaders/vertexShader.glsl","shaders/fragmentShader.glsl");

    //TEXTURES 
	
	stbi_set_flip_vertically_on_load(true);

    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    int x,y,nrChannel;
    unsigned char* data = stbi_load("images/container.jpg",&x,&y,&nrChannel,0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,x,y,0,GL_RGB,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    else {
        std::cout<<"FAILED TO LOAD IMAGE!"<<std::endl;
    }
    stbi_image_free(data);
    
    unsigned int texture2;
    glGenTextures(1,&texture2);
    glBindTexture(GL_TEXTURE_2D,texture2);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    data = stbi_load("images/awesomeface.png",&x,&y,&nrChannel,0);

    if (data){
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,x,y,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout<<"FAILED TO LOAD IMAGE!"<<std::endl;
    }
    stbi_image_free(data);
    
    //3 1
    //2 0

    //making buffers
	float verts[] = {
		// positions			   // texture coords
		-0.5f, -0.5f, -0.5f,       0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,       1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,       1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,       1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,       0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,       0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,       0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,       1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,       1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,       1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,       0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,       0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,       1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,       1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,       0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,       0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,       0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,       1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,       1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,       1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,       0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,       0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,       0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,       1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,       0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,       1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,       1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,       1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,       0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,       0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,       0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,       1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,       1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,       1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,       0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,       0.0f, 1.0f
	};

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

    //unsigned int indices[]={
    //   0,1,2,
    //   1,2,3
    //};

    // unsigned int indices[] ={};
    unsigned int VBO,VAO;//EBO;

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    //glGenBuffers(1,&EBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);

    //vertex attrib pointers .....
    
    //coords
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    //Texture
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    shader.use();
	glUniform1i(glad_glGetUniformLocation(shader.ID , "ourTexture") , 0);
	glUniform1i(glad_glGetUniformLocation(shader.ID , "ourTexture2") , 1);
    glBindTexture(GL_TEXTURE_2D,texture);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnable(GL_DEPTH_TEST);
	

	float currentFrame,lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f,0.3f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D , texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D , texture2);

		glUniform1f(glGetUniformLocation(shader.ID , "changer") , changer);
	
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		//glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,3.0f);
		//glm::vec3 cameraTarget = glm::vec3(0.0f , 0.0f , 0.0f);
		//glm::vec3 cameraDir = glm::normalize(cameraPos - cameraTarget);
		//glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f , 1.0f, 0.0f), cameraDir));
		//glm::vec3 cameraUp = glm::cross(cameraDir , cameraRight);
	
		//Calculate Delta Time
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        glBindVertexArray(VAO);


		glm::mat4 view = look_at(cameraPos,cameraPos + cameraFront , cameraUp);
		//view = glm::translate(view, glm::vec3(0.0f, sinf(glfwGetTime()) , -3.0f));

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		for(unsigned int i = 0 ; i < 10;i++){			
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			angle *= (float)glfwGetTime();
			model = glm::rotate(model , glm::radians(angle) , glm::vec3(1.0f, 0.3f, 0.5f) );
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES , 0, 36);
		}
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0; 
}
