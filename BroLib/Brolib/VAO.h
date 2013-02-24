#ifndef __VAO_H__
#define __VAO_H__

#include <gl/glew.h>
#include "VBO.h"


template<class T>
class VAO
{
private:
	GLuint vao;
	VAO(const VAO &other);
public:
	VAO(VBO<T>* vbo)
	{
		glGenVertexArrays(1, &vao);
		bind();
		vbo->bind();
		vbo->setVAO();
	}

	~VAO()
	{
		glDeleteVertexArrays(1, &vao);
	}

	void bind()
	{
		glBindVertexArray(vao);
	}

	void unBind()
	{
		glBindVertexArray(0);
	}



};



#endif