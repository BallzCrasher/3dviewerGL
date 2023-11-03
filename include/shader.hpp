#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/ext.hpp>
#include <filesystem>
#include <string_view>

struct Shader {
    unsigned int ID;

    Shader(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath);
    void use();
    void cleanUp();

    GLint getUniformLocation(std::string_view name);
    bool setFloat      (std::string_view name, float f);
    bool setInt        (std::string_view name, int f);
    bool setUnsignedInt(std::string_view name, unsigned int f);

    bool setVec2  (std::string_view name, const glm::vec2&   v,                         size_t count = 1);
    bool setVec3  (std::string_view name, const glm::vec3&   v,                         size_t count = 1);
    bool setVec4  (std::string_view name, const glm::vec4&   v,                         size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat2&   m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat3&   m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat4&   m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat2x3& m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat3x2& m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat4x2& m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat2x4& m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat4x3& m, bool transpose = false, size_t count = 1);
    bool setMatrix(std::string_view name, const glm::mat3x4& m, bool transpose = false, size_t count = 1);
};

#endif 
