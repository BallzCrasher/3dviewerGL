#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/ext.hpp>
#include <shader.hpp>

Shader::Shader(const char* vertexShaderPath,const char* fragmentShaderPath){
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
	catch(std::ifstream::failure& e)
	{
		std::cerr << "ERROR COULDN'T OPEN FILES " << '\n';
		std::cerr << e.what() << ' ' << e.code() << '\n';
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
		std::cout<<"ERROR::VERTEX::SHADER:: "<<infoLog<<std::endl;
	}
	
	glGetShaderiv(fragShader,GL_COMPILE_STATUS,&success);
	if (!success){
		glGetShaderInfoLog(fragShader,512,NULL,infoLog);
		std::cout<<"ERROR::FRAGMENT::SHADER:: "<<infoLog<<std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID,vertShader);
	glAttachShader(ID,fragShader);
	glLinkProgram(ID);

	glGetProgramiv(ID,GL_LINK_STATUS,&success);
	if (!success){
		glGetProgramInfoLog(ID,512,NULL,infoLog);
		std::cout<<"ERROR ::PROGRAM::LINK::"<<infoLog<<std::endl;
	}
}

int Shader::getUniformLocation(const char* name) {
  auto result = glGetUniformLocation(this->ID, name);
  if (result == -1)
    std::cerr << "Shader[id = " << ID 
      << "]: Error::Couldn't find uniform" << name << std::endl;
  return result;
}

/** numbers **/
bool Shader::setFloat(const char* name, float value) {
  auto pos = getUniformLocation(name);
  glUniform1f(pos, value);
  return pos != -1;
}

bool Shader::setInt(const char* name, int value) {
  auto pos = getUniformLocation(name);
  glUniform1i(pos, value);
  return pos != -1;
}

bool Shader::setUnsignedInt(const char* name, unsigned int value) {
  auto pos = getUniformLocation(name);
  glUniform1ui(pos, value);
  return pos != -1;
}


/** vectors **/
bool Shader::setVec2(const char* name, const glm::vec2& v, size_t count) {
  auto pos = getUniformLocation(name);
  glUniform2fv(pos, count, glm::value_ptr(v));
  return pos != -1;
}

bool Shader::setVec3(const char* name, const glm::vec3& v, size_t count) {
  auto pos = getUniformLocation(name);
  glUniform3fv(pos, count, glm::value_ptr(v));
  return pos != -1;
}

bool Shader::setVec4(const char* name, const glm::vec4& v, size_t count) {
  auto pos = getUniformLocation(name);
  glUniform4fv(pos, count, glm::value_ptr(v));
  return pos != -1;
}


/** matricies **/
bool Shader::setMatrix(const char* name, const glm::mat2& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix2fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat3& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix3fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat4& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix4fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat2x3& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix2x3fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat3x2& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix3x2fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat4x2& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix4x2fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat2x4& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix2x4fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat4x3& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix4x3fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}

bool Shader::setMatrix(const char* name, const glm::mat3x4& m, bool transpose, size_t count) {
  auto pos = getUniformLocation(name);
  glUniformMatrix3x4fv(pos, count, transpose, glm::value_ptr(m));
  return pos != -1;
}


void Shader::use(){
	glUseProgram(ID);
}

void Shader::cleanUp(){
	glDeleteProgram(ID);
}
