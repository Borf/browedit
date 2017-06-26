#pragma once

#include <glm/glm.hpp>

namespace blib { struct MouseState; struct KeyState; }

class Camera
{
public:
	bool ortho;
	float distance;
	float direction;
	glm::vec2 position;
	glm::vec2 targetPosition;


	Camera(void);
	virtual ~Camera(void);

	virtual glm::mat4 getMatrix() const = 0;
	virtual void update(double elapsedTime);
	virtual void setTarget(const glm::vec2 &target) {
		this->targetPosition = target;
	};

	virtual void handleMouse(const blib::MouseState& mouseState, const blib::MouseState &lastMouseState, const blib::KeyState &keyState) = 0;
};

class ModernCamera : public Camera
{
public:
	float angle;

	ModernCamera();
	~ModernCamera();
	virtual glm::mat4 getMatrix() const;
	virtual void handleMouse(const blib::MouseState& mouseState, const blib::MouseState &lastMouseState, const blib::KeyState &keyState);
};


class ClassicCamera : public Camera
{
public:
	float direction;
	float increment = 0;

	virtual glm::mat4 getMatrix() const override;
	virtual void handleMouse(const blib::MouseState& mouseState, const blib::MouseState &lastMouseState, const blib::KeyState &keyState);
	virtual void setTarget(const glm::vec2 &target) {
		this->targetPosition = target;
	};
};
