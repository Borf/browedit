#pragma once

#include <Brolib/Camera.h>

class BrowCamera : public Camera
{
	glm::vec3 lookAt;
public:
	BrowCamera();
	~BrowCamera();
	virtual glm::mat4 getMatrix();

};

