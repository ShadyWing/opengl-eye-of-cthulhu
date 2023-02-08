#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>

#include <vector>

class VBO
{
public:
	unsigned int ID;

	template<typename T>
	VBO(const std::vector<T>& vertices)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), &vertices[0], GL_STATIC_DRAW);
	}
	~VBO()
	{
		unBind();
		Delete();
	}

	void bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	void unBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Delete()
	{
		glDeleteBuffers(1, &ID);
	}
};

#endif
