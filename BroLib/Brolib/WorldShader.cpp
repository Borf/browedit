#include "WorldShader.h"

#include <stdio.h>
WorldShader::WorldShader() : Shader("data/shaders/world.vert", "data/shaders/world.frag")
{
	bindAttributeLocation("a_position", 0);
	bindAttributeLocation("a_normal", 1);
	bindAttributeLocation("a_color", 2);
	bindAttributeLocation("a_texCoord1", 3);
	bindAttributeLocation("a_texCoord2", 4);
	bindFragLocation("fragColor", 0);
	link();
}


WorldShader::~WorldShader(void)
{
}

void WorldShader::setProjectionMatrix( glm::mat4 matrix )
{
	setUniform("projectionMatrix", matrix);
}

void WorldShader::setViewMatrix( glm::mat4 matrix )
{
	setUniform("viewMatrix", matrix);
}

void WorldShader::setModelMatrix( glm::mat4 matrix )
{
	setUniform("modelMatrix", matrix);
}


