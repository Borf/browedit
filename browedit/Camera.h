#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(void);
	~Camera(void);

	glm::vec2 position;
	float direction;
	float angle;
	float distance;

	glm::mat4 getMatrix() const;

};

