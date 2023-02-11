#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>

#include <myinclude/VBO.h>

class VAO
{
public:
	unsigned int ID;

	VAO()
	{
		glGenVertexArrays(1, &ID);
	}
	~VAO()
	{
		unBind();
		Delete();
	}

	inline void linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
	{
		VBO.bind();
		// ���Ӷ�������		 ��x��attribute��x�����ݣ��������ͣ�x����������ʼƫ��
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.unBind();
	}

	inline void bind()
	{
		glBindVertexArray(ID);
	}

	inline void unBind()
	{
		glBindVertexArray(0);
	}

	inline void Delete()
	{
		glDeleteVertexArrays(1, &ID);
	}
};
#endif