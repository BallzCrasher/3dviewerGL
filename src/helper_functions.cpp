#ifndef H_HELPER_FUNCTIONS
#define H_HELPER_FUNCTIONS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "model.hpp"
#include "shader.hpp"
#include "stb_image.h"
#include "variables.hpp"

void toggleFlashlight(){ 
	flashLight_switch = !flashLight_switch;
}

std::ostream& operator<<(std::ostream& stream, glm::mat4 mat){
	stream << std::fixed << std::setprecision(2);
	for(int i= 0; i < 4;i++){
		stream << "| "; 
		for(int j = 0;j < 4; j++) std::cout << mat[i][j] << ' '; 
		stream << " |\n";
	}
	stream << std::endl;
	return stream;
}

unsigned int load_texture(const char* path){	
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

	return textureID;
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

void setPointLight(unsigned int shaderID, size_t index 
		,glm::vec3 light_position
		,glm::vec3 light_ambient,glm::vec3 light_diffuse, glm::vec3 light_specular
		,float constant , float linear , float quadratic)
{ 
	std::string s_pos   = "pointLights[" + std::to_string(index) + "].position";
	std::string s_amb   = "pointLights[" + std::to_string(index) + "].ambient";
	std::string s_dif   = "pointLights[" + std::to_string(index) + "].diffuse";
	std::string s_spec  = "pointLights[" + std::to_string(index) + "].specular";
	std::string s_const = "pointLights[" + std::to_string(index) + "].constant";
	std::string s_lin   = "pointLights[" + std::to_string(index) + "].linear";
	std::string s_quad  = "pointLights[" + std::to_string(index) + "].quadratic";

	glUniform3fv(glGetUniformLocation(shaderID,s_pos .c_str()), 1, glm::value_ptr(light_position));
	glUniform3fv(glGetUniformLocation(shaderID,s_amb .c_str()), 1, glm::value_ptr(light_ambient));
	glUniform3fv(glGetUniformLocation(shaderID,s_dif .c_str()), 1, glm::value_ptr(light_diffuse));
	glUniform3fv(glGetUniformLocation(shaderID,s_spec.c_str()), 1, glm::value_ptr(light_specular));
	glUniform1f(glGetUniformLocation(shaderID,s_const.c_str()) ,constant);
	glUniform1f(glGetUniformLocation(shaderID,s_lin  .c_str()) ,linear);
	glUniform1f(glGetUniformLocation(shaderID,s_quad .c_str()) ,quadratic);
}

void setSpotLight(unsigned int shaderID
		,glm::vec3 light_position , glm::vec3 light_direction
		,glm::vec3 light_ambient  , glm::vec3 light_diffuse , glm::vec3 light_specular
		,float constant , float linear , float quadratic
		,float cutoff, float outerCutoff)
{
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.position"), 1, glm::value_ptr(light_position));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.direction"), 1, glm::value_ptr(light_direction));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.ambient"), 1, glm::value_ptr(light_ambient));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.diffuse"), 1, glm::value_ptr(light_diffuse));
	glUniform3fv(glGetUniformLocation(shaderID,"spotLight.specular"), 1, glm::value_ptr(light_specular));
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.constant") ,constant);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.linear") ,linear);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.quadratic") ,quadratic);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.cutoff") ,cutoff);
	glUniform1f (glGetUniformLocation(shaderID,"spotLight.outerCutoff") ,outerCutoff);
}

void setDirectionalLight(unsigned int shaderID ,glm::vec3 light_direction
		,glm::vec3 light_ambient  , glm::vec3 light_diffuse , glm::vec3 light_specular)
{
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.direction"), 1, glm::value_ptr(light_direction));
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.ambient"), 1, glm::value_ptr(light_ambient));
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.diffuse"), 1, glm::value_ptr(light_diffuse));
	glUniform3fv(glGetUniformLocation(shaderID,"dirLight.specular"), 1, glm::value_ptr(light_specular));
}


void drawObject_outlined(const Model& object ,Shader& shader ,Shader& outline_shader,
				  glm::mat4 model, const glm::mat4& view, const glm::mat4& projection,const glm::vec3& scaleVector)
//color is controlled though outline_shader
{ 
		//mask the container
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
		glStencilFunc(GL_ALWAYS,1,0xFF);
		glStencilMask(0xFF);
		object.draw(shader);

		//draw only where the fragment isn't masked
		glStencilFunc(GL_NOTEQUAL,1,0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		//Draw scaled up container 
		outline_shader.use();
		model = glm::scale(model,scaleVector);
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"view"),1,GL_FALSE,glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(outline_shader.ID,"model"),1,GL_FALSE,glm::value_ptr(model));
		object.draw(outline_shader);
		
		//undo the stuff to draw other models
		//glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS,1,0xFF);
		glEnable(GL_DEPTH_TEST);

		glStencilMask(0x00);
}
#endif
