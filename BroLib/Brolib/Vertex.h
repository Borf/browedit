#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <gl/glew.h>
#include <gl/GL.h>

#include <glm/glm.hpp>

class Vertex
{
public:
};


class VertexPosition : public Vertex
{
public:
	glm::vec3 position;

	VertexPosition(glm::vec3 position)
	{
		this->position = position;
	}

	static int getSize()
	{
		return sizeof(GL_FLOAT) * 3;
	}
	static void setPointer(int totalSize)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, totalSize, 0);
	}
	static void unsetPointer()
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	static int setVAO(int totalSize)
	{
		int index = 0;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, totalSize, (void*) 0);
		return index;
	}
};

class VertexPositionColor : public VertexPosition
{
public:
	glm::vec4 color;


	VertexPositionColor(glm::vec3 position, glm::vec4 color) : VertexPosition(position)
	{
		this->color = color;
	}

	static int getSize()
	{
		return VertexPosition::getSize() + sizeof(GL_FLOAT)*4;
	}
	static void setPointer(int totalSize)
	{
		VertexPosition::setPointer(totalSize);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, totalSize, (void*)VertexPosition::getSize());
	}
	static void unsetPointer()
	{
		VertexPosition::unsetPointer();
		glDisableClientState(GL_COLOR_ARRAY);
	}
	static int setVAO(int totalSize)
	{
		int index = VertexPosition::setVAO(totalSize)+1;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, totalSize, (void*) VertexPosition::getSize());
		return index;
	}
};

class VertexPositionColorNormal : public VertexPositionColor
{
public:
	glm::vec3 normal;


	VertexPositionColorNormal(glm::vec3 position, glm::vec4 color, glm::vec3 normal) : VertexPositionColor(position, color)
	{
		this->normal = normal;
	}
	static int getSize()
	{
		return VertexPositionColor::getSize() + sizeof(GL_FLOAT)*3;
	}
	static void setPointer(int totalSize)
	{
		VertexPositionColor::setPointer(totalSize);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, totalSize, (void*)VertexPositionColor::getSize());
	}
	static void unsetPointer()
	{
		VertexPositionColor::unsetPointer();
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	static int setVAO(int totalSize)
	{
		int index = VertexPositionColor::setVAO(totalSize)+1;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, totalSize, (void*) VertexPositionColor::getSize());
		return index;
	}
};

class VertexPositionNormal : public VertexPosition
{
public:
	glm::vec3 normal;


	VertexPositionNormal(glm::vec3 position, glm::vec3 normal) : VertexPosition(position)
	{
		this->normal = normal;
	}
	static int getSize()
	{
		return VertexPosition::getSize() + sizeof(GL_FLOAT)*3;
	}
	static void setPointer(int totalSize)
	{
		VertexPosition::setPointer(totalSize);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, totalSize, (void*)VertexPosition::getSize());
	}
	static void unsetPointer()
	{
		VertexPosition::unsetPointer();
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	static int setVAO(int totalSize)
	{
		int index = VertexPosition::setVAO(totalSize)+1;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, totalSize, (void*) VertexPosition::getSize());
		return index;
	}
};


class VertexPositionColorTexture : public VertexPositionColor
{
public:
	glm::vec2 texCoord;

	VertexPositionColorTexture(glm::vec3 position, glm::vec4 color, glm::vec2 texCoord) : VertexPositionColor(position, color)
	{
		this->texCoord = texCoord;
	}
	static int getSize()
	{
		return VertexPositionColor::getSize() + sizeof(GL_FLOAT)*2;
	}
	static void setPointer(int totalSize)
	{
		VertexPosition::setPointer(totalSize);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, totalSize, (void*) VertexPositionColor::getSize());
	}
	static void unsetPointer()
	{
		VertexPositionColor::unsetPointer();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	static int setVAO(int totalSize)
	{
		int index = VertexPositionColor::setVAO(totalSize)+1;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, totalSize, (void*) VertexPositionColor::getSize());
		return index;
	}
};

class VertexPositionColorNormalTexture : public VertexPositionColorNormal
{
public:
	glm::vec2 texCoord;

	VertexPositionColorNormalTexture(glm::vec3 position, glm::vec4 color, glm::vec3 normal, glm::vec2 texCoord) : VertexPositionColorNormal(position, color, normal)
	{
		this->texCoord = texCoord;
	}
	
	static int getSize()
	{
		return VertexPositionColorNormal::getSize() + sizeof(GL_FLOAT)*2;
	}
	static void setPointer(int totalSize)
	{
		VertexPositionColorNormal::setPointer(totalSize);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, totalSize, (void*) VertexPositionColorNormal::getSize());
	}
	static void unsetPointer()
	{
		VertexPositionColorNormal::unsetPointer();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	static int setVAO(int totalSize)
	{
		int index = VertexPositionColorNormal::setVAO(totalSize)+1;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, totalSize, (void*) VertexPositionColorNormal::getSize());
		return index;
	}

};


class VertexPositionNormalTexture : public VertexPositionNormal
{
public:
	glm::vec2 texCoord;

	VertexPositionNormalTexture(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord) : VertexPositionNormal(position, normal)
	{
		this->texCoord = texCoord;
	}

	static int getSize()
	{
		return VertexPositionNormal::getSize() + sizeof(GL_FLOAT)*2;
	}
	static void setPointer(int totalSize)
	{
		VertexPositionNormal::setPointer(totalSize);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, totalSize, (void*) VertexPositionNormal::getSize());
	}
	static void unsetPointer()
	{
		VertexPositionNormal::unsetPointer();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	static int setVAO(int totalSize)
	{
		int index = VertexPositionNormal::setVAO(totalSize)+1;
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, totalSize, (void*) VertexPositionNormal::getSize());
		return index;
	}

};

#endif
