#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>

#include <vector>

class EBO
{
public:
	unsigned int ID;

	EBO(std::vector<unsigned int>& indices)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	}
	~EBO()
	{
		unBind();
		Delete();
	}

	void bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	}

	void unBind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Delete()
	{
		glDeleteBuffers(1, &ID);
	}
};
#endif
