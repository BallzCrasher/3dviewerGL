#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "shader.hpp"
#include "stb_image.h"
#include "model.hpp"
#include "helper.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <assimp/material.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


static std::vector<Texture> textures_loaded; 

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures){ 
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) , &vertices[0] , GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int) , &indices[0] , GL_STATIC_DRAW);

	//vertex pointers
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void *)0 );
	glEnableVertexAttribArray(0);

	//normal
	glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	//texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, texCoords)));
	glEnableVertexAttribArray(2);
}

void Mesh::draw(Shader& shader) const { 
	unsigned int diffuseNum = 1;
	unsigned int specularNum = 1;
	for(unsigned int i= 0; i < textures.size(); i++){ 
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNum++);
		else if (name == "texture_specular"){ 
			number = std::to_string(specularNum++);
		} else { 
			std::cerr << "Invalid Texture Name : " <<  name << std::endl;
		}

		glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()) , i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT , 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}
	
Model::Model(std::filesystem::path path, bool gamma) : gammaCorrection(gamma)
{
	loadModel(path);
}

void Model::draw(Shader& shader) const {  
  for (const auto& mesh : meshs)
    mesh.draw(shader);
}
	
void Model::loadModel(std::filesystem::path path){ 
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.parent_path();

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) { 
	for(size_t i = 0; i < node->mNumMeshes; i++){ 
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshs.push_back(processMesh(mesh, scene));
	}

	for(size_t i = 0; i < node->mNumChildren; i++){ 
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) { 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	
	for(size_t i = 0; i < mesh->mNumVertices; i++){ 
		Vertex vertex;
		glm::vec3 tvector;
		tvector.x = mesh->mVertices[i].x;
		tvector.y = mesh->mVertices[i].y;
		tvector.z = mesh->mVertices[i].z;
		vertex.position = tvector;

		if (mesh->HasNormals()){ 
			tvector.x = mesh->mNormals[i].x;
			tvector.y = mesh->mNormals[i].y;
			tvector.z = mesh->mNormals[i].z;
			vertex.normal = tvector;
		}

		if(mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x; 
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		} else { 
			vertex.texCoords = glm::vec2(0.0f, 0.0f);  
		}

		vertices.push_back(vertex);
	}

	for(size_t i = 0 ; i < mesh->mNumFaces;i++){ 
		aiFace face = mesh->mFaces[i];
		for(size_t j = 0; j < face.mNumIndices;j++){ 
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex > 0) { 
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,std::string typeName) {
	std::vector<Texture> textures;
	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for(unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if(std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) // equal
			{
				textures.push_back(textures_loaded[j]);
				skip = true; 
				break;
			}
		}

		if(!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = load_texture(directory / str.C_Str());
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}
