#ifndef H_HELPER_FUNCTIONS
#define H_HELPER_FUNCTIONS
#include <cstdio>
#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <chrono>
#include <string>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "model.hpp"
#include "shader.hpp"
#include "variables.hpp"

void toggleFlashlight();
std::ostream& operator<<(std::ostream& stream, glm::mat4 mat);
std::ostream& operator<<(std::ostream& stream, glm::vec3 v);
unsigned int load_texture(const char* path);
glm::mat4 look_at(glm::vec3 position ,glm::vec3 target,glm::vec3 world_up);
void setPointLight(unsigned int shaderID, size_t index 
 ,glm::vec3 light_position
 ,glm::vec3 light_ambient,glm::vec3 light_diffuse, glm::vec3 light_specular
 ,float constant , float linear , float quadratic);
void setSpotLight(unsigned int shaderID
 ,glm::vec3 light_position , glm::vec3 light_direction
 ,glm::vec3 light_ambient  , glm::vec3 light_diffuse , glm::vec3 light_specular
 ,float constant , float linear , float quadratic
 ,float cutoff, float outerCutoff);
void setDirectionalLight(unsigned int shaderID ,glm::vec3 light_direction
 ,glm::vec3 light_ambient  , glm::vec3 light_diffuse , glm::vec3 light_specular);
void drawObject_outlined(const Model& object ,Shader& shader ,Shader& outline_shader,
				  glm::mat4 model, const glm::mat4& view, const glm::mat4& projection,const glm::vec3& scaleVector);
#endif
