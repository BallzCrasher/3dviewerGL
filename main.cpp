#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include <math.h>
#include <chrono>
#include "includes/glm/fwd.hpp"
#include "includes/glm/glm.hpp"
#include "includes/glm/gtc/matrix_transform.hpp"
#include "includes/glm/gtc/type_ptr.hpp"
#include "includes/Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

float changer = 0.01f;

void framebuffer_size_callback(GLFWwindow* window, int width , int height){
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow* window){
    if (glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
        changer += 0.01f;
        if (changer > 0.99) changer -= 0.01f;
    }
    if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
        changer -= 0.01f;
        if (changer < 0.01) changer += 0.01f;
    }
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

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,600,"OpenGL Window",NULL,NULL);
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
    // positions          // colors           // texture coords
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[]={
        0,1,2,
        1,2,3
    };

    // unsigned int indices[] ={};
    unsigned int VBO,VAO,EBO;

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);

    //vertex attrib pointers .....
    
    //coords
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    //color
    glVertexAttribPointer(1,3,GL_FLOAT,false,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //Texture
    glVertexAttribPointer(2,2,GL_FLOAT,false,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    shader.use();
	glUniform1i(glad_glGetUniformLocation(shader.ID , "ourTexture") , 0);
	glUniform1i(glad_glGetUniformLocation(shader.ID , "ourTexture2") , 1);
    glBindTexture(GL_TEXTURE_2D,texture);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glm::mat4 trans(1.0f);
    trans = glm::rotate(trans , glm::radians(90.0f) , glm::vec3(0.0f,0.0f,1.0f));
	trans = glm::scale(trans,glm::vec3(0.5,0.5,0.5));

	glUniformMatrix4fv(glGetUniformLocation(shader.ID,"transform")
                            ,1,GL_FALSE,glm::value_ptr(trans));
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f,0.3f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);

        //float time = std::chrono::milliseconds;
        //float lol1= sin(rand())/8,lol2 = cosf(rand())/8;

		glUniform1f(glGetUniformLocation(shader.ID , "changer") , changer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D , texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D , texture2);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0; 
}
