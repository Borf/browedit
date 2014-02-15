#pragma once

#include <blib/App.h>
#include <BroLib/MapRenderer.h>

namespace blib { 
	class Texture; 
	namespace wm { class WM; class Menu; }
}

class Map;

class BrowEdit : public blib::App
{
	blib::wm::WM* wm;
	blib::wm::Menu* rootMenu;

	Map* map;
	MapRenderer mapRenderer;

public:
	BrowEdit(void);
	~BrowEdit(void);

	virtual void init();
	virtual void update( double elapsedTime );
	virtual void draw();

	void loadMap(std::string fileName);
	void saveMap(std::string fileName);

};

