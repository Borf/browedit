#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

#include <blib/App.h>
#include <blib/util/Log.h>

using blib::util::Log;

Camera::Camera()
{
	ortho = false;
	distance = 50;
}

Camera::~Camera()
{
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

ModernCamera::ModernCamera()
{
	position = glm::vec2(0,0);
	direction = 0;
	angle = 60;
	Log::out << "Creating modern camera" << Log::newline;
}


ModernCamera::~ModernCamera()
{
	Log::out << "Disposing modern camera" << Log::newline;
}

glm::mat4 ModernCamera::getMatrix() const
{
	glm::mat4 ret;
	ret = glm::translate(ret, glm::vec3(0,0,-distance));
	ret = glm::rotate(ret, glm::radians(angle), glm::vec3(1, 0, 0));
	ret = glm::rotate(ret, glm::radians(direction), glm::vec3(0, 1, 0));
	ret = glm::translate(ret, glm::vec3(-position.x, 0, -position.y));

	return ret;
}


void ModernCamera::handleMouse(const blib::MouseState & mouseState, const blib::MouseState & lastMouseState, const blib::KeyState & keyState)
{
	if (keyState.isPressed(blib::Key::SHIFT))
	{
		if (mouseState.clickcount == 2)
			angle = 60;
		direction += (mouseState.position.x - lastMouseState.position.x) / 2.0f;
		angle = glm::clamp(angle + (mouseState.position.y - lastMouseState.position.y) / 2.0f, 0.0f, 90.0f);
	}
	else
	{
		if (mouseState.clickcount == 2)
			direction = 0;
		position -= glm::vec2(glm::vec4(mouseState.position.x - lastMouseState.position.x, mouseState.position.y - lastMouseState.position.y, 0, 0) * glm::rotate(glm::mat4(), -glm::radians(direction), glm::vec3(0, 0, 1)));
		targetPosition = position;
	}
}














glm::mat4 ClassicCamera::getMatrix() const
{
	glm::mat4 ret;
	
	ret = glm::lookAt(glm::vec3(position.x, 0, position.y) + 
		glm::vec3(distance * cos(glm::radians(direction+90.0f)), distance, distance * sin(glm::radians(direction+90.0f))),
						glm::vec3(position.x, increment, position.y),
						glm::vec3(0, 1, 0));
	return ret;
}


void ClassicCamera::handleMouse(const blib::MouseState & mouseState, const blib::MouseState & lastMouseState, const blib::KeyState & keyState)
{
	if (keyState.isPressed(blib::Key::SHIFT))
	{
		if (keyState.isPressed(blib::Key::CONTROL))
		{
			increment = glm::clamp(increment - (mouseState.position.y - lastMouseState.position.y) / 2.0f, -1000.0f, 1000.0f);
			direction += (mouseState.position.x - lastMouseState.position.x) / 2.0f;
		}
		else
		{
			if (mouseState.clickcount == 2)
				direction = 60;
			
			direction += (mouseState.position.x - lastMouseState.position.x) / 2.0f;
			distance = glm::clamp(distance + (mouseState.position.y - lastMouseState.position.y) / 2.0f, 0.0f, 2000.0f);
		}
	}
	else
	{
		position -= glm::vec2(glm::vec4(mouseState.position.x - lastMouseState.position.x, mouseState.position.y - lastMouseState.position.y, 0, 0) * glm::rotate(glm::mat4(), -glm::radians(direction), glm::vec3(0, 0, 1)));
		targetPosition = position;
	}

}

