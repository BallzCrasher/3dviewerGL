#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/ext.hpp>

struct Shader {
    unsigned int ID;

    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
    void use();
    void cleanUp();

    GLint getUniformLocation(const char* name);
    bool setFloat(const char* name, float f);
    bool setInt(const char* name, int f);
    bool setUnsignedInt(const char* name, unsigned int f);

    bool setVec2(const char* name, const glm::vec2& v, size_t count = 1);
    bool setVec3(const char* name, const glm::vec3& v, size_t count = 1);
    bool setVec4(const char* name, const glm::vec4& v, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat2& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat3& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat4& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat2x3& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat3x2& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat4x2& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat2x4& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat4x3& m, bool transpose = false, size_t count = 1);
    bool setMatrix(const char* name, const glm::mat3x4& m, bool transpose = false, size_t count = 1);
};

#endif 
