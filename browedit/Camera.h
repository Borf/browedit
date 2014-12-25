#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(void);
	~Camera(void);

	glm::vec2 position;
	glm::vec2 targetPosition;
	float direction;
	float angle;
	float distance;
	bool ortho;

	glm::mat4 getMatrix() const;
	void update(double elapsedTime);
};

