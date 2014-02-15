#pragma once

#include <glm/glm.hpp>

class Map;

class MapRenderer
{
public:
	glm::mat4 cameraMatrix;

	bool drawShadows;
	bool drawObjects;
	bool drawTextureGrid;
	bool drawObjectGrid;
	bool drawQuadTree;


	void render(const Map* map);

};
