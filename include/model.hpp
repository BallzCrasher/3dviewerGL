#ifndef MODEL_H
#define MODEL_H 

#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <assimp/material.h>
#include <assimp/types.h>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.hpp"
#include "stb_image.h"

struct Vertex{ 
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture{ 
	unsigned int id;
	std::string type;
	std::filesystem::path path;
};

struct Mesh{ 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,std::vector<Texture> textures);

	void draw(Shader& shader) const;
	
private:
	unsigned int VBO , VAO , EBO;
};

struct Model { 
  Model(std::filesystem::path path, bool gamma = false);

	void draw(Shader& shader) const;
	
private:
	std::vector<Mesh> meshs;
	std::filesystem::path directory;
	bool gammaCorrection;

	void loadModel(std::filesystem::path path);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};



#endif /* ifndef MODEL_H */
