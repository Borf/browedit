#pragma once

#include <json/value.h>
#include <Brolib/Singleton.h>
#include <Brolib/Camera.h>
#include <BroLib/Shader.h>
#include <BroLib/WorldShader.h>
#include <BroLib/World.h>
#include <glm/glm.hpp>

class BrowEdit : public Singleton<BrowEdit>
{
public:
	BrowEdit(void);
	~BrowEdit(void);

	Json::Value config;
	int width;
	int height;


	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	Camera* camera;
	World* world;
	WorldShader* worldShader;




	void init();
	void draw();
	void reshape(int width, int height);
	void update();
	void keyboard(char key);
	void specialKey(int key);
};

