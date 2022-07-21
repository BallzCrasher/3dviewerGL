#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include "glm/vec3.hpp"

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexShaderPath,const char* fragmentShaderPath){
        std::string fragmentShaderCode;
        std::string vertexShaderCode;
        std::ifstream vertFile;
        std::ifstream fragFile;
        try
        {
            vertFile.open(vertexShaderPath);
            fragFile.open(fragmentShaderPath);
            std::stringstream vss,fss;
            vss << vertFile.rdbuf();
            fss << fragFile.rdbuf();
            vertFile.close();
            fragFile.close();
            fragmentShaderCode = fss.str();
            vertexShaderCode = vss.str();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR COULDN'T OPEN FILES " << '\n';
        }
        const char* vertCode = vertexShaderCode.c_str();
        const char* fragCode = fragmentShaderCode.c_str();

        unsigned int fragShader,vertShader;
        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader,1,&vertCode,NULL);
        glShaderSource(fragShader,1,&fragCode,NULL);
        glCompileShader(fragShader);
        glCompileShader(vertShader);
        char infoLog[512];
        int success;
        glGetShaderiv(vertShader,GL_COMPILE_STATUS,&success);
        if (!success){
            glGetShaderInfoLog(vertShader,512,NULL,infoLog);
            std::cout<<"ERRPR::VERTEX::SHADER:: "<<infoLog<<std::endl;
        }
        
        glGetShaderiv(fragShader,GL_COMPILE_STATUS,&success);
        if (!success){
            glGetShaderInfoLog(fragShader,512,NULL,infoLog);
            std::cout<<"ERRPR::FRAGMENT::SHADER:: "<<infoLog<<std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID,vertShader);
        glAttachShader(ID,fragShader);
        glLinkProgram(ID);

        glGetProgramiv(ID,GL_LINK_STATUS,&success);
        if (!success){
            glGetProgramInfoLog(ID,512,NULL,infoLog);
            std::cout<<"ERRPR ::PROGRAM::LINK::"<<infoLog<<std::endl;
        }

    }
    void use(){
        glUseProgram(ID);
    }
    
    void cleanUp(){
        glDeleteProgram(ID);
    }
    void setVec3Uniform(const char* name,float v1,float v2,float v3){
        glUniform3f(glGetUniformLocation(ID,name),v1,v2,v3);
    }

    void setVec3Uniform(const char* name,glm::vec3 v){
        glUniform3fv(glGetUniformLocation(ID,name),1, &v[0]);
    }
};
