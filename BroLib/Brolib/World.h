#pragma once

#include <string>

#include <Brolib/VBO.h>
#include <Brolib/VAO.h>
#include <BroLib/Vertex.h>

#include "Worldshader.h"
#include "Camera.h"


class World
{
	VBO<VertexPosition> vbo;
	VAO<VertexPosition>* vao;

public:
	World(std::string fileName);
	~World(void);

	void draw(Camera* camera, WorldShader* shader);
};

