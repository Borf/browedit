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
class ObjectWindow;


class BrowEdit : public blib::App, public blib::MouseListener
{
	blib::wm::WM* wm;
	blib::wm::Menu* rootMenu;

	enum class EditMode
	{
		TextureEdit,
		ObjectEdit,
		GatEdit,
		WallEdit,
	};

	EditMode editMode;

	MapRenderer mapRenderer;
	Json::Value config;

	blib::MouseState lastMouseState;
	blib::MouseState startMouseState;
	blib::KeyState lastKeyState;

	blib::RenderState highlightRenderState;
	enum class HighlightShaderUniforms
	{
		s_texture,
		modelviewMatrix,
		projectionMatrix,
		color,
		texMult,
	};


	blib::RenderState composeRenderState;
	enum class ComposeShaderUniforms
	{
		s_texture,
		s_texture2,
	};


	glm::vec4 mouse3dstart;
	glm::vec4 lastmouse3d;


	TextureWindow* textureWindow;
	ObjectWindow* objectWindow;
public:
	BrowEdit(const Json::Value &config);
	~BrowEdit(void);

	virtual void init();
	virtual void update( double elapsedTime );
	virtual void draw();

	void loadMap(std::string fileName);
	void saveMap(std::string fileName);

	virtual bool onScroll( int delta );


	void setEditMode(EditMode newMode);



	Map* map;
	Camera* camera;

	glm::ivec2 textureTargetSize;
	int textureRot;
	bool textureFlipH;
	bool textureFlipV;

};

