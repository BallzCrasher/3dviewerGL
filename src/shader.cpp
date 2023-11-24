#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>
#include <array>
#include <iostream>
#include <glm/ext.hpp>
#include <shader.hpp>
#include <filesystem>

/**
 * Shader constructor. 
 * This will compile and link the vertex shader and the fragment shader.
 * And will create a program and assign it a unique id.
 * any errors generated through this process will be sent to stdout.
 *
 * @param vertex_shader_path the path to the vertex shader code.
 * @param fragment_shader_path the path to the fragment shader code.
 */
Shader::Shader(
		std::filesystem::path vertex_shader_path,
		std::filesystem::path fragment_shader_path) 
{
	std::string fragment_shader_code;
	std::string vertex_shader_code;
	std::ifstream vert_file;
	std::ifstream frag_file;
	try
	{
		vert_file.open(vertex_shader_path);
		frag_file.open(fragment_shader_path);
		std::stringstream vss, fss;
		vss << vert_file.rdbuf();
		fss << frag_file.rdbuf();
		vert_file.close();
		frag_file.close();
		fragment_shader_code = fss.str();
		vertex_shader_code = vss.str();
	}
	catch(std::ifstream::failure& e)
	{
		std::cerr << "ERROR::COULDN'T OPEN FILES." << '\n';
		std::cerr << e.what() << ' ' << e.code() << '\n';
	}
	auto vert_code {vertex_shader_code.c_str()};
	auto frag_code {fragment_shader_code.c_str()};

	unsigned int frag_shader {glCreateShader(GL_FRAGMENT_SHADER)}; 
	unsigned int vert_shader {glCreateShader(GL_VERTEX_SHADER)};
	glShaderSource(vert_shader, 1, &vert_code, NULL);
	glShaderSource(frag_shader, 1, &frag_code, NULL);
	glCompileShader(frag_shader);
	glCompileShader(vert_shader);
  std::array<char, 512> info_log;
	int success {};
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log.data());
		std::cout << "ERROR::VERTEX::SHADER::" << info_log.data() << std::endl;
	}
	
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log.data());
		std::cout << "ERROR::FRAGMENT::SHADER::" << info_log.data() << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vert_shader);
	glAttachShader(ID, frag_shader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success){
		glGetProgramInfoLog(ID, 512, NULL, info_log.data());
		std::cout << "ERROR ::PROGRAM::LINK::" << info_log.data() << std::endl;
	}
}

int Shader::getUniformLocation(std::string_view name) {
  auto result = glGetUniformLocation(this->ID, name.data());
  if (result == -1)
    std::cerr << "Shader[id = " << ID 
      << "]: Error::Couldn't find uniform" << name << std::endl;
  return result;
}

/** numbers **/
bool Shader::setFloat(std::string_view name, float value) {
  auto pos = getUniformLocation(name);
  glUniform1f(pos, value);
  return pos != -1;
}

bool Shader::setInt(std::string_view name, int value) {
  auto pos = getUniformLocation(name);
  glUniform1i(pos, value);
  return pos != -1;
}

bool Shader::setUnsignedInt(std::string_view name, unsigned int value) {
  auto pos = getUniformLocation(name);
  glUniform1ui(pos, value);
  return pos != -1;
}


/** vectors **/
bool Shader::setVec2(std::string_view name, const glm::vec2& v, size_t count) {
  auto pos = getUniformLocation(name);
  glUniform2fv(pos, count, glm::value_ptr(v));
  return pos != -1;
}

bool Shader::setVec3(std::string_view name, const glm::vec3& v, size_t count) {
  auto pos = getUniformLocation(name);
  glUniform3fv(pos, count, glm::value_ptr(v));
  return pos != -1;
}

bool Shader::setVec4(std::string_view name, const glm::vec4& v, size_t count) {
  auto pos = getUniformLocation(name);
  glUniform4fv(pos, count, glm::value_ptr(v));
  return pos != -1;
}


/** matricies **/
bool Shader::setMatrix(std::string_view name, const glm::mat2& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix2fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat3& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix3fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat4& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix4fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat2x3& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix2x3fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat3x2& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix3x2fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat4x2& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix4x2fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat2x4& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix2x4fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat4x3& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix4x3fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(std::string_view name, const glm::mat3x4& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix3x4fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}


/**
 * set the current shader program to be the this shader.
 * shader.use() must be used before setting uniforms in the shader.
 * And also must be used before drawing with this shader.
 */
void Shader::use() {
	glUseProgram(ID);
}

Shader::~Shader(){
	glDeleteProgram(ID);
}
