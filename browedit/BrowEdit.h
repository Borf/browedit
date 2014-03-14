#pragma once

#include <blib/App.h>
#include <blib/MouseListener.h>
#include <BroLib/MapRenderer.h>
#include <json/value.h>

namespace blib { 
	class Texture; 
	namespace wm { class WM; class Menu; }
}

class Map;
class Camera;
class TextureWindow;


class BrowEdit : public blib::App, public blib::MouseListener
{
	blib::wm::WM* wm;
	blib::wm::Menu* rootMenu;

	Map* map;
	Camera* camera;
	MapRenderer mapRenderer;
	Json::Value config;

	blib::MouseState lastMouseState;


	TextureWindow* textureWindow;
public:
	BrowEdit(const Json::Value &config);
	~BrowEdit(void);

	virtual void init();
	virtual void update( double elapsedTime );
	virtual void draw();

	void loadMap(std::string fileName);
	void saveMap(std::string fileName);

	virtual bool onScroll( int delta );

};

