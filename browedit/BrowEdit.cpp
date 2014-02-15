#include "BrowEdit.h"

#include <BroLib/GrfFileSystemHandler.h>
#include <BroLib/Map.h>

#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>
#include <blib/BackgroundTask.h>

#include <blib/wm/WM.h>
#include <blib/util/FileSystem.h>


BrowEdit::BrowEdit(void)
{
	appSetup.width = 1280;
	appSetup.height = 720;
	appSetup.vsync = true;
	appSetup.icon = 0;
	appSetup.renderer = blib::AppSetup::GlRenderer;
	appSetup.border = true;


	map = NULL;
}


BrowEdit::~BrowEdit(void)
{
}

void BrowEdit::init()
{
	wm = blib::wm::WM::getInstance();
	wm->setRadialMenu(wm->loadMenu("assets/menu.json"));

	config = blib::util::FileSystem::getJson("assets/configs/config.laptop.json");

	std::list<blib::BackgroundTask*> tasks;
	for(Json::ArrayIndex i = 0; i < config["data"]["grfs"].size(); i++)
		tasks.push_back(new blib::BackgroundTask(NULL, [this,i]() { blib::util::FileSystem::registerHandler(new GrfFileSystemHandler(config["data"]["grfs"][i].asString())); }));
	for(blib::BackgroundTask* task : tasks)
		task->waitForTermination();
	//TODO: make sure registerHandle is threadsafe!



	loadMap("data/prontera");
}

void BrowEdit::update( double elapsedTime )
{
	if(keyState.isPressed(blib::KEY_ESC))
		running = false;
}

void BrowEdit::draw()
{
	renderer->clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), blib::Renderer::Color | blib::Renderer::Depth);

	if(map)
		mapRenderer.render(map);


	spriteBatch->begin();
	wm->draw(*spriteBatch);
	spriteBatch->end();

}

void BrowEdit::loadMap(std::string fileName)
{
	if(map)
		delete map;

	Map** newMap = new Map*;

	runBackground([newMap, fileName] () 
	{ *newMap = new Map(fileName); }, 
	[this, newMap] () 
	{ map = *newMap; delete newMap; } );
}