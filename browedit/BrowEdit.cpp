#include "BrowEdit.h"
#include "Camera.h"
#include "windows/FileOpenWindow.h"

#include <BroLib/GrfFileSystemHandler.h>
#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>
#include <blib/BackgroundTask.h>

#include <blib/wm/WM.h>
#include <blib/util/FileSystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



BrowEdit::BrowEdit(const Json::Value &config)
{
	this->config = config;

	appSetup.width = config["resolution"][0u].asInt();
	appSetup.height = config["resolution"][1u].asInt();
	appSetup.vsync = false;
	appSetup.icon = 0;
	appSetup.renderer = blib::AppSetup::GlRenderer;
	appSetup.border = true;
	appSetup.title = "BrowEdit 2.0";

	appSetup.threaded = true;
	appSetup.vsync = false;


	map = NULL;
	wm = blib::wm::WM::getInstance();
}


BrowEdit::~BrowEdit(void)
{
}

void BrowEdit::init()
{
	std::list<blib::BackgroundTask<int>*> tasks;
	for (Json::ArrayIndex i = 0; i < config["data"]["grfs"].size(); i++)
		tasks.push_back(new blib::BackgroundTask<int>(NULL, [this, i]() { blib::util::FileSystem::registerHandler(new GrfFileSystemHandler(config["data"]["grfs"][i].asString())); return 0; }));
	for (blib::BackgroundTask<int>* task : tasks)
		task->waitForTermination();
	//TODO: make sure registerHandle is threadsafe!, make sure the background tasks are cleaned up


	wm->setSkin("assets/skins/ro.json", resourceManager);
	wm->setRadialMenu(wm->loadMenu("assets/menu.json"));
	addMouseListener(wm);
	addKeyListener(wm);


	//new FileOpenWindow(resourceManager, this);


	addMouseListener(this);
	



//	loadMap("data/c_tower1");
	loadMap("data/prontera");
//	loadMap("data/yuno");

	mapRenderer.init(resourceManager, this);
	camera = new Camera();

}

void BrowEdit::update( double elapsedTime )
{
	if(keyState.isPressed(blib::KEY_ESC))
		running = false;

	mapRenderer.cameraMatrix = camera->getMatrix();

	if(mouseState.middleButton)
	{
		if(keyState.isPressed(blib::KEY_SHIFT))
		{
			camera->direction += (mouseState.x - lastMouseState.x) / 2.0f;
			camera->angle = glm::clamp(camera->angle + (mouseState.y - lastMouseState.y) / 2.0f, 0.0f, 90.0f);
		}
		else
			camera->position -= glm::vec2(glm::vec4(mouseState.x - lastMouseState.x, mouseState.y - lastMouseState.y,0,0) * glm::rotate(glm::mat4(), -camera->direction, glm::vec3(0,0,1)));
	}

	lastMouseState = mouseState;
}

void BrowEdit::draw()
{
	renderer->clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), blib::Renderer::Color | blib::Renderer::Depth);

	if(map)
	{
		mapRenderer.render(renderer);
	}

	spriteBatch->begin();
	wm->draw(*spriteBatch);
	spriteBatch->end();

}

void BrowEdit::loadMap(std::string fileName)
{
	if(map)
		delete map;
	map = NULL;

	runBackground<Map*>(	[fileName] () { return new Map(fileName); }, 
							[this] (Map* param) { map = param;
										camera->position = glm::vec2(map->getGnd()->width*5, map->getGnd()->height*5);
										mapRenderer.setMap(map);
	} );
}

bool BrowEdit::onScroll( int delta )
{
	camera->distance -= delta/10.0f;
	return true;
}
