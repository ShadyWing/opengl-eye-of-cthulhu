#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <stb_image.h>

#include <myinclude/shader.h>

#include <string>
using namespace std;

unsigned int loadTexture(const char* imagePath);

class Texture
{
public:
	unsigned int ID;
	string type;
	string path;

	Texture(const char* imagePath, string texType)
	{
		type = texType;

		ID = loadTexture(imagePath);
	}
	Texture()
	{

	}
	~Texture()
	{
		unBind();
		Delete();
	}
	
	void setSlot(Shader& shader, const char* uniform, GLuint uint)
	{
		shader.bind();
		shader.setInt(uniform, uint);
		shader.unBind();
	}

	void bind(GLenum texIndex = GL_TEXTURE0)
	{
		glActiveTexture(texIndex);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void unBind(GLenum texIndex = GL_TEXTURE0)
	{
		glActiveTexture(texIndex);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
	}

	void Delete()
	{
		glActiveTexture(GL_TEXTURE0);
		glDeleteTextures(1, &ID);
	}
};

unsigned int loadTexture(const char* imagePath)
{
	unsigned int texID;

	// 纹理图像
	int width, height, numColorChannel;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(imagePath, &width, &height, &numColorChannel, 0);
	stbi_set_flip_vertically_on_load(false);
	if (data)
	{
		GLenum colorChannel;
		if (numColorChannel == 1)
			colorChannel = GL_RED;
		if (numColorChannel == 3)
			colorChannel = GL_RGB;
		if (numColorChannel == 4)
			colorChannel = GL_RGBA;

		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		// 设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//x,x,读进来是什么通道，x,x,x,源文件是什么通道
		glTexImage2D(GL_TEXTURE_2D, 0, colorChannel, width, height, 0, colorChannel, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}
#endif