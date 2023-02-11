#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <myinclude/shader.h>
#include <myinclude/mesh.h>
#include <myinclude/camera.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

enum impSetting {
	eNULL			 = 0x0,
	eGammaCorrection = 0x1,
	eFlipUVs		 = 0x2,
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model {
public:
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;
	bool flipUVs;

	Model(const char* path, unsigned int setting = eFlipUVs) :
		gammaCorrection(setting >> 0 & 1),
		flipUVs((setting >> 1) & 1)
	{
		mTranslate = glm::translate(mTranslate, glm::vec3(0.0f));
		mRotate = glm::toMat4(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
		mScale = glm::scale(mScale, glm::vec3(1.0f));

		loadModel(path);
	}

	void Draw(Shader& shader, Camera& camera)
	{
		updateModelMatrix();
		mView = camera.getViewMatrix();
		mProjection = camera.getProjectionMatrix();

		shader.setMat4("model", mModel);
		shader.setMat4("view", mView);
		shader.setMat4("projection", mProjection);

		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	// 相对变换
	void rTranslate(glm::vec3 translate)
	{
		mTranslate = glm::translate(mTranslate, translate);
		updateModelMatrix();
	}

	void rRotate(const float degree, const glm::vec3 axis)
	{
		mRotate = glm::toMat4(glm::angleAxis(glm::radians(degree), glm::normalize(axis))) * mRotate;
		updateModelMatrix();
	}

	void rScale(glm::vec3 scale)
	{
		mScale = glm::scale(mScale, scale);
		updateModelMatrix();
	}

	// 重设变换
	void translate(glm::vec3 translate)
	{
		glm::mat4 newtrans;
		mTranslate = glm::translate(newtrans, translate);
		updateModelMatrix();
	}

	void rotate(const float degree, const glm::vec3 axis)
	{
		mRotate = glm::toMat4(glm::angleAxis(glm::radians(degree), glm::normalize(axis)));
		updateModelMatrix();
	}

	void scale(glm::vec3 scale)
	{
		glm::mat4 newscale;
		mScale = glm::scale(newscale, scale);
		updateModelMatrix();
	}

private:
	glm::mat4 mTranslate;
	glm::mat4 mRotate;
	glm::mat4 mScale;
			  
	glm::mat4 mModel;
	glm::mat4 mView;
	glm::mat4 mProjection;

	void loadModel(const string& path)
	{
		Assimp::Importer import;
		const aiScene* scene;
		if(flipUVs)
			scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		else
			scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		vector<Vertex>		 vertices;
		vector<unsigned int> indices;
		vector<Texture>		 textures;

		// 顶点
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;

				// TexCoords
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f);

			vertices.push_back(vertex);
		}

		// 索引
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// 材质
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			vector<Texture> alphaMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_alpha");
			textures.insert(textures.end(), alphaMaps.begin(), alphaMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture texture;
				texture.ID = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}

	void updateModelMatrix()
	{
		mModel = mTranslate * mRotate * mScale;
	}
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum colorChannel; 
		switch (nrChannels) {
		case 1:
			colorChannel = GL_RED; break;
		case 3:
			colorChannel = GL_RGB; break;
		case 4:
			colorChannel = GL_RGBA; break;
		default:break;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		//x,x,读进来是什么通道，x,x,x,源文件是什么通道
		glTexImage2D(GL_TEXTURE_2D, 0, colorChannel, width, height, 0, colorChannel, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// 设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}
#endif