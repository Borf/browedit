#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(void)
{
	position = glm::vec2(0,0);
	direction = 0;
	angle = 60;
	distance = 50;

	ortho = false;
}


Camera::~Camera(void)
{
}

glm::mat4 Camera::getMatrix() const
{
	glm::mat4 ret;
	ret = glm::translate(ret, glm::vec3(0,0,-distance));
	ret = glm::rotate(ret, glm::radians(angle), glm::vec3(1, 0, 0));
	ret = glm::rotate(ret, glm::radians(direction), glm::vec3(0, 1, 0));
	ret = glm::translate(ret, glm::vec3(-position.x, 0, -position.y));

	return ret;
}

void Camera::update(double elapsedTime)
{
	glm::vec2 diff = targetPosition - position;

	float length = glm::length(diff);

	float dist = (float)elapsedTime * 1000;


	if (length < dist)
		position = targetPosition;
	else
		position += diff / length * dist;

	
}
