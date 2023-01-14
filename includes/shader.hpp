#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/vec3.hpp>

struct Shader {
    unsigned int ID;

    Shader(const char* vertexShaderPath,const char* fragmentShaderPath);
    void use();
    
    void cleanUp();
    void setVec3Uniform(const char* name,float v1,float v2,float v3);

    void setVec3Uniform(const char* name,glm::vec3 v);
};

#endif 
