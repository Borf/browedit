#pragma once

#include "Shader.h"

#include <glm/glm.hpp>


class WorldShader : public Shader
{
public:
	WorldShader();
	~WorldShader();


	void setProjectionMatrix(glm::mat4 matrix);
	void setViewMatrix(glm::mat4 matrix);
	void setModelMatrix(glm::mat4 matrix);
};

