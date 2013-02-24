#ifndef __VIO_H__
#define __VIO_H__

#include <Windows.h>
#include <gl/glew.h>
#include <gl/GL.h>


template <class T>
class VIO
{
private:
	GLuint vio;

	T* element;
	int length;
	VIO(const VIO &other)
	{
		throw "do not copy!";
	}

public:
	VIO()
	{
		length = 0;
		element = NULL;
		if(GLEW_VERSION_1_5)
			glGenBuffers(1, &vio);
		else
			glGenBuffersARB(1, &vio);
	}
	~VIO()
	{
		if(GLEW_VERSION_1_5)
			glDeleteBuffers(1, &vio);
		else
			glDeleteBuffersARB(1, &vio);
	}

	void setData(int length, T* data, GLenum usage)
	{
		this->length = length;
		bind();
		if(GLEW_VERSION_1_5)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(T) * length, data, usage);
		else
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(T) * length, data, usage);
	}


	void bind()
	{
		if(GLEW_VERSION_1_5)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
		else
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vio);
	}

	void unBind()
	{
		if(GLEW_VERSION_1_5)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		else
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
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
			element = (T*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, access);
		else
			element = (T*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, access);
		return element;
	}
	void unmapData()
	{
		bind();
		if(GLEW_VERSION_1_5)
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		else
			glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
		element = NULL;
	}

	T& operator [](int index)
	{
		if(element == NULL)
			throw "Use mapData before accessing";
		return element[index];
	}
};


#endif