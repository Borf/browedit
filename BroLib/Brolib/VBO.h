#ifndef __VBO_H__
#define __VBO_H__

#include <Windows.h>
#include <gl/glew.h>
#include <gl/GL.h>


template <class T>
class VBO
{
private:
	GLuint vbo;

	T* element;
	int length;
	VBO(const VBO &other)
	{
		throw "do not copy!";
	}

public:
	VBO()
	{
		length = 0;
		element = NULL;
		if(GLEW_VERSION_1_5)
			glGenBuffers(1, &vbo);
		else
			glGenBuffersARB(1, &vbo);
	}
	~VBO()
	{
		if(GLEW_VERSION_1_5)
			glDeleteBuffers(1, &vbo);
		else
			glDeleteBuffersARB(1, &vbo);
	}

	void setData(int length, T* data, GLenum usage)
	{
		this->length = length;
		bind();
		if(GLEW_VERSION_1_5)
			glBufferData(GL_ARRAY_BUFFER, T::getSize() * length, data, usage);
		else
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, T::getSize() * length, data, usage);
	}


	void bind()
	{
		if(GLEW_VERSION_1_5)
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
		else
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	}

	void unBind()
	{
		if(GLEW_VERSION_1_5)
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		else
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	int elementSize()
	{
		return T::getSize();
	}

	int getLength()
	{
		return length;
	}

	T* mapData(GLenum access)
	{
		bind();
		if(GLEW_VERSION_1_5)
			element = (T*)glMapBuffer(GL_ARRAY_BUFFER, access);
		else
			element = (T*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, access);
		return element;
	}
	void unmapData()
	{
		bind();
		if(GLEW_VERSION_1_5)
			glUnmapBuffer(GL_ARRAY_BUFFER);
		else
			glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
		element = NULL;
	}

	T& operator [](int index)
	{
		if(element == NULL)
			throw "Use mapData before accessing";
		return element[index];
	}


	void setPointer()
	{
		T::setPointer(T::getSize());
	}
	void unsetPointer()
	{
		T::unsetPointer();
	}


	void setVAO()
	{
		T::setVAO(T::getSize());
	}

};


#endif