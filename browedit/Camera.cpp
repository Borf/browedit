#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(void)
{
	position = glm::vec2(0,0);
	direction = 0;
	angle = 45;
	distance = 200;
}


Camera::~Camera(void)
{
}

glm::mat4 Camera::getMatrix() const
{
	glm::mat4 ret;
	ret = glm::translate(ret, glm::vec3(0,0,-distance));
	ret = glm::rotate(ret, angle, glm::vec3(1,0,0));
	ret = glm::rotate(ret, direction, glm::vec3(0,1,0));
	ret = glm::translate(ret, glm::vec3(-position.x,-50, -position.y));

	return ret;
}
