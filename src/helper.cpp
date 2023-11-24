#ifndef H_HELPER_FUNCTIONS
#define H_HELPER_FUNCTIONS
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>

#include "model.hpp"
#include "shader.hpp"
#include "stb_image.h"
#include "variables.hpp"
#include "helper.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


// for printing mat4
std::ostream& operator<<(std::ostream& stream, glm::mat4 mat){
	stream << std::fixed << std::setprecision(2);
	for(int i= 0; i < 4;i++){
		stream << "| "; 
		for(int j = 0;j < 4; j++) std::cout << mat[i][j] << ' '; 
		stream << " |\n";
	}
	return stream;
}

// for printing vec3
std::ostream& operator<<(std::ostream& stream, glm::vec3 v){
	stream << std::fixed << std::setprecision(2);
	stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return stream;
}

/**
 * will toggel the flashlight. 
 * this will invert GlobalVars::flashlight_switch.
 */
void toggle_flashlight() { 
	GlobalVars::flashlight_switch = !GlobalVars::flashlight_switch;
}

/**
 * Load texture from the filesystem and bind it to a GL texture id.
 * if there is an error. This function will print it out to stdout.
 * And will return -1 (unsigned);
 *
 * @param path Path to the texture
 * @return The GL id of the texture.
 */
unsigned int load_texture(std::filesystem::path path){	
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  auto data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
		return -1;
  }
  return textureID;
}


/**
 * returns a look at matrix when looking from position at target.
 *
 * @param position Coordinates of the position that you are looking from.
 * @param target Coordinates of the position that you are looking at.
 * @param world_up A normalized vector that points up in your world.
 * @return returns a look at matrix when looking from position at target.
 */

glm::mat4 look_at(glm::vec3 position, glm::vec3 target, glm::vec3 world_up){
	glm::vec3 zaxis {glm::normalize(position - target)};
	glm::vec3 xaxis {glm::normalize(glm::cross(world_up, zaxis))};
	glm::vec3 yaxis {glm::cross(zaxis, xaxis)};

	glm::mat4 left {glm::transpose(glm::mat4(
		glm::vec4(xaxis,0.0f),
		glm::vec4(yaxis,0.0f),
		glm::vec4(zaxis,0.0f),
		glm::vec4(0.0f,0.0f,0.0f,1.0f))
	)};

	glm::mat4 right {glm::translate(glm::mat4(1.0f), -position)};
	
	return left * right;
}


/**
 * Sets point light properties to the given shader.
 * shader.use() must be called before this function.
 * the shader must have an array called pointLights of light objects
 * of the form:
 * struct {
 *   vec3 position;
 *
 *   vec3 ambient;
 *   vec3 diffuse;
 *   vec3 specular;
 *
 *   float constant;
 *   float linear;
 *   float quadratic;
 * };
 * the constant, linear and quadratic variables are the terms to
 * the Attenuation formula.
 * Attenuation = 1 / (constant + linear * d + quadratic * d * d)
 * where d is the distance between the light source and the fragment.
 *
 * @param shader The shader that will be setting the light to.
 * @param index The index of the light in thepointLights array.
 * @param light_position The position of the light.
 * @param light_ambient The Ambient property of the light.
 * @param light_diffuse The Diffuse property of the light.
 * @param light_specular The Specular property of the light.
 * @param constant The constant term of the Attenuation formula.
 * @param linear The linear term of the Attenuation formula.
 * @param quadratic The quadratic term of the Attenuation formula.
 */
void set_point_light(Shader& shader, size_t index,
		glm::vec3 light_position,
		glm::vec3 light_ambient, glm::vec3 light_diffuse, glm::vec3 light_specular,
		float constant , float linear , float quadratic)
{ 
	std::string s_pos   = "pointLights[" + std::to_string(index) + "].position";
	std::string s_amb   = "pointLights[" + std::to_string(index) + "].ambient";
	std::string s_dif   = "pointLights[" + std::to_string(index) + "].diffuse";
	std::string s_spec  = "pointLights[" + std::to_string(index) + "].specular";
	std::string s_const = "pointLights[" + std::to_string(index) + "].constant";
	std::string s_lin   = "pointLights[" + std::to_string(index) + "].linear";
	std::string s_quad  = "pointLights[" + std::to_string(index) + "].quadratic";

	shader.setVec3(s_pos,    light_position);
	shader.setVec3(s_amb,    light_ambient);
	shader.setVec3(s_dif,    light_diffuse);
	shader.setVec3(s_spec,   light_specular);
	shader.setFloat(s_const, constant);
	shader.setFloat(s_lin,   linear);
	shader.setFloat(s_quad,  quadratic);
}

/**
 * Sets spot light properties to the given shader.
 * shader.use() must be called before this function.
 * The spot light object in the shader must be of the form:
 * struct {
 *   vec3 position;
 *   vec3 direction;
 *
 *   vec3 ambient;
 *   vec3 diffuse;
 *   vec3 specular;
 *
 *   float constant;
 *   float linear;
 *   float quadratic;
 *   float cutoff;
 *   float outerCutoff;
 * };
 * and the light object must be called spotLight in the shader.
 * 
 * the constant, linear and quadratic variables are the terms to
 * the Attenuation formula.
 * Attenuation = 1 / (constant + linear * d + quadratic * d * d)
 * where d is the distance between the light source and the fragment.
 * 
 * The angles of cutoff and outerCutoff are measured in radians.
 *
 * @param shader The shader that will be setting the light to.
 * @param light_position The position of the light.
 * @param light_direction The direction that the spot light is directed to.
 * @param light_ambient The Ambient property of the light.
 * @param light_diffuse The Diffuse property of the light.
 * @param light_specular The Specular property of the light.
 * @param constant The constant term of the Attenuation formula.
 * @param linear The linear term of the Attenuation formula.
 * @param quadratic The quadratic term of the Attenuation formula.
 * @param cutoff The angle of which the spot light circle starts to fade.
 * @param outerCutoff The angle of which the spot light circle fades completely.
 */
void set_spot_light(Shader& shader,
		glm::vec3 light_position, glm::vec3 light_direction,
		glm::vec3 light_ambient, glm::vec3 light_diffuse, glm::vec3 light_specular,
		float constant, float linear, float quadratic,
		float cutoff, float outerCutoff)
{
	shader.setVec3("spotLight.position",      light_position);
	shader.setVec3("spotLight.direction",     light_direction);
	shader.setVec3("spotLight.ambient",       light_ambient);
	shader.setVec3("spotLight.diffuse",       light_diffuse);
	shader.setVec3("spotLight.specular",      light_specular);
	shader.setFloat("spotLight.constant",     constant);
	shader.setFloat("spotLight.linear",       linear);
	shader.setFloat("spotLight.quadratic",    quadratic);
	shader.setFloat("spotLight.cutoff",       glm::cos(cutoff));
	shader.setFloat("spotLight.outerCutoff",  glm::cos(outerCutoff));
}

/**
 * Sets directional light properties to the given shader.
 * shader.use() must be called before this function.
 * The shader must have a struct of the form:
 * struct {
 *   vec3 direction;
 *
 *   vec3 ambient;
 *   vec3 diffuse;
 *   vec3 specular;
 * };
 * and the light object must be called dirLight in the shader.
 *
 * @param shader The shader that will be setting the light to.
 * @param light_position The position of the light.
 * @param light_direction The direction of the light.
 * @param light_ambient The Ambient property of the light.
 * @param light_diffuse The Diffuse property of the light.
 * @param light_specular The Specular property of the light.
 */
void set_directional_light(Shader& shader, 
		glm::vec3 light_direction,
		glm::vec3 light_ambient, glm::vec3 light_diffuse, glm::vec3 light_specular)
{
	shader.setVec3("dirLight.direction", light_direction);
	shader.setVec3("dirLight.ambient",   light_ambient);
	shader.setVec3("dirLight.diffuse",   light_diffuse);
	shader.setVec3("dirLight.specular",  light_specular);
}


/**
 * draws an outlined object.
 * This works by drawing the initial object then drawing a scaled up version
 * of it and displaying it only in the parts where the initial object
 * isn't there.
 * GL_STENCIL_TEST must be enabled for this to work.
 * also GL_STENCIL_BUFFER_BIT must be added to glClear in the start of the loop.
 * The color is controlled though outline_shader and must be set before
 * this function is called.
 * This function will also set glStencilOp to (GL_KEEP, GL_KEEP, GL_REPLACE).
 * @param object the 3d object that you want to be drawn outlined
 * @param shader the shader of the object
 * @param outline_shader the shader of the outline
 * @param model the model matrix of the object.
 * @param view the view matrix.
 * @param projection the projection matrix.
 * @param scale_vector the vector of which the outline object will be scaled to.
 */
void draw_outlined(const Model& object, 
		Shader& shader, Shader& outline_shader,
		glm::mat4 model, 
		const glm::mat4& view,
		const glm::mat4& projection,
		const glm::vec3& scale_vector)
{ 
		//mask the container
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		object.draw(shader);

		//draw only where the fragment isn't masked
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		//Draw scaled up container 
		outline_shader.use();
		model = glm::scale(model, scale_vector);
		outline_shader.setMatrix("view", view);
		outline_shader.setMatrix("projection", projection);
		outline_shader.setMatrix("model", model);
		object.draw(outline_shader);
		
		//undo the stuff to draw other models
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
}
#endif
