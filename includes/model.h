#ifndef MODEL_H
#define MODEL_H 

#include "glm/glm.hpp"
#include "glm/fwd.hpp"
#include "Shader.h"
#include <assimp/material.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

struct Vertex{ 
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture{ 
	unsigned int id;
	std::string type;
	std::string path;
};

struct Mesh{ 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices,std::vector<unsigned int> indices,std::vector<Texture> textures){ 
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

	void Draw(Shader& shader){ 
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
				std::cerr << "Invalid Texture Name : " + name << std::endl;
			}

			glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()) , i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT , 0);
		glBindVertexArray(0);
	}
	
private:
	unsigned int VBO , VAO , EBO;
};

struct Model{ 

	static std::vector<Texture> textures_loaded; 
	std::vector<Mesh> meshs;
	std::string directory;

	Model(char* path){ 

	}
	void draw(Shader& shader){ 
		for(int i = 0; i < meshs.size(); i++)
			meshs[i].Draw(shader);
	}
	
	void loadModel(std::string path){ 
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0,path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene){ 
		for(int i = 0 ; i < node->mNumMeshes; i++){ 
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshs.push_back(processMesh(mesh, scene));
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene){ 
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		
		for(int i = 0; i < mesh->mNumVertices; i++){ 
			Vertex vertex;
			glm::vec3 tvector;
			tvector.x = mesh->mVertices[i].x;
			tvector.y = mesh->mVertices[i].y;
			tvector.z = mesh->mVertices[i].z;
			vertex.position = tvector;

			tvector.x = mesh->mNormals[i].x;
			tvector.y = mesh->mNormals[i].y;
			tvector.z = mesh->mNormals[i].z;
			vertex.normal = tvector;

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

		for(int i = 0 ; i < mesh->mNumFaces;i++){ 
			aiFace face = mesh->mFaces[i];
			for(int j = 0; j < face.mNumIndices;j++){ 
				indices.push_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0){ 
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,std::string typeName){ 
		std::vector<Texture> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for(unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; 
					break;
				}
			}
			if(!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // add to loaded textures
			}
		}
	}

	unsigned int load_texture(const char* texture_path){	
		unsigned int texture;
		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D,texture);
		int x,y,nrChannel;
		unsigned char* data = stbi_load(texture_path,&x,&y,&nrChannel,0);
		if (data){
			GLenum format;
				if (nrChannel == 1)
					format = GL_RED;
				else if (nrChannel == 3)
					format = GL_RGB;
				else if (nrChannel == 4)
					format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,x,y,0,format,GL_UNSIGNED_BYTE,data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		} else {
			std::cout<<"FAILED TO LOAD IMAGE!"<<std::endl;
		}
		stbi_image_free(data);
		return texture;
	}

	static unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
		}

		return textureID;
	}

};


#endif /* ifndef MODEL_H */
