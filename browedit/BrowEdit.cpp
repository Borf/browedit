#include "BrowEdit.h"

#include <BroLib/Map.h>

#include <blib/wm/WM.h>
#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>


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

	loadMap("prontera");
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