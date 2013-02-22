#include "BrowEdit.h"
#include <gl/glew.h>
#include <gl/GL.h>

#include <glm/gtc/matrix_transform.hpp>


BrowEdit::BrowEdit(void)
{
}


BrowEdit::~BrowEdit(void)
{
	config = Json::nullValue;
}







void BrowEdit::draw()
{
	glClearColor(0.1f, 0.2f, 0.05f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//viewMatrix = camera->getMatrix();

	projectionMatrix = glm::perspective(80.0f, width/(float)height, 0.1f, 1000.0f);
	viewMatrix = glm::lookAt(glm::vec3(0,0,-10), glm::vec3(0,0,0), glm::vec3(0,1,0));
	modelMatrix = glm::mat4();



}

void BrowEdit::reshape(int width, int height)
{
	this->width = width;
	this->height = height;
	glViewport(0,0,width,height);
}

void BrowEdit::update()
{

}

void BrowEdit::keyboard( char key )
{

}

void BrowEdit::specialKey( int key )
{

}
