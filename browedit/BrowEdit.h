#pragma once

#include <blib/App.h>
#include <blib/MouseListener.h>
#include <BroLib/MapRenderer.h>
#include <json/value.h>
#include "TranslatorTool.h"
#include "RotateTool.h"
#include "ScaleTool.h"

namespace blib { 
	class Texture; 
	namespace wm { class WM; class Menu; }
}

class Map;
class Camera;
class TextureWindow;
class ObjectWindow;

class Action;
class ObjectEditAction;
class SelectObjectAction;

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
		HeightEdit,
	};

	EditMode editMode;

	Json::Value config;

	blib::MouseState lastMouseState;
	blib::MouseState startMouseState;
	blib::KeyState lastKeyState;

	blib::RenderState highlightRenderState;
	blib::RenderState composeRenderState;
	enum class ComposeShaderUniforms
	{
		s_texture,
		s_texture2,
		sampleSize,
	};


	std::list<Action*> actions;
	std::list<Action*> undone;

	glm::vec4 mouse3dstart;
	glm::vec4 lastmouse3d;

	blib::math::Ray mouseRay;

	blib::Texture* gradientBackground;

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
	MapRenderer mapRenderer;
	Camera* camera;

	glm::ivec2 textureTargetSize;
	int textureRot;
	bool textureFlipH;
	bool textureFlipV;


	TranslatorTool translatorTool;
	TranslatorTool::Axis objectTranslateDirection;

	RotatorTool rotatorTool;
	RotatorTool::Axis objectRotateDirection;

	ScaleTool scaleTool;
	ScaleTool::Axis objectScaleDirection;

	std::vector<ObjectEditAction*> objectEditActions;
	SelectObjectAction* selectObjectAction;

	enum class ObjectEditModeTool
	{
		Translate,
		Rotate,
		Scale,
	} objectEditModeTool;
	enum class HighlightShaderUniforms
	{
		s_texture,
		modelviewMatrix,
		projectionMatrix,
		normalMatrix,
		diffuse,
		color,
		texMult,
	};


	void setObjectEditMode(ObjectEditModeTool newMode);
	void addModel(const std::string &fileName);

	void perform(Action* action);
	void undo();
	void redo();

	Rsw::Model* newModel;
};

