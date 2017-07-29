#pragma once

#include <blib/App.h>
#include <blib/MouseListener.h>
#include <BroLib/MapRenderer.h>
#include <blib/json.hpp>
#include "TranslatorTool.h"
#include "RotateTool.h"
#include "ScaleTool.h"

#include <v8.h>

namespace blib { 
	class Texture; 
	namespace wm { class WM; class Menu; class ToggleMenuItem; }
}

class Map;
class Camera;
class TextureWindow;
class ObjectWindow;
class ColorWindow;

class Action;
class ObjectEditAction;
class SelectObjectAction;


class JsRunner
{
public:
	v8::Persistent<v8::Object> obj;
	v8::Persistent<v8::Function> func;
	v8::Isolate* isolate;

	void run();

	int counter = 4;

};

class BrowEdit : public blib::App, public blib::MouseListener
{

	enum class EditMode
	{
		TextureEdit,
		ObjectEdit,
		WallEdit,
		HeightEdit,
		DetailHeightEdit,
		GatEdit,
		DetailGatEdit,
		GatTypeEdit,
		LightmapEdit,
		ColorEdit,
	};

	bool stupidOlrox = false;
	EditMode editMode;

	json config;
	json translation;

	blib::MouseState lastMouseState;
	blib::MouseState startMouseState;
	blib::KeyState lastKeyState;

	blib::RenderState highlightRenderState;
	blib::RenderState composeRenderState;
	enum class ComposeShaderUniforms
	{
		s_texture,
		s_texture2,
	};


	std::list<Action*> actions;
	std::list<Action*> undone;

	glm::vec4 mouse3dstart;
	glm::vec4 lastmouse3d;

	blib::math::Ray mouseRay;

	blib::Texture* gradientBackground;

public:
	TextureWindow* textureWindow;
	ObjectWindow* objectWindow;
	ColorWindow* colorWindow;


	BrowEdit(const json &config, v8::Isolate* isolate);
	~BrowEdit(void);

	virtual void init();
	virtual void update( double elapsedTime );

	void textureEditUpdate();
	void objectEditUpdate();

	virtual void draw();

	virtual bool onScroll( int delta );


	void setEditMode(EditMode newMode);


	blib::wm::WM* wm;
	blib::wm::Menu* rootMenu;

	Map* map;
	MapRenderer mapRenderer;
	Camera* camera;

	glm::ivec2 textureTargetSize;
	int textureRot;
	bool textureFlipH;
	bool textureFlipV;
	inline const json &getConfig() { return config; }

	//object edit
	TranslatorTool translatorTool;
	TranslatorTool::Axis objectTranslateDirection;

	RotatorTool rotatorTool;
	RotatorTool::Axis objectRotateDirection;

	ScaleTool scaleTool;
	ScaleTool::Axis objectScaleDirection;

	std::vector<ObjectEditAction*> objectEditActions;
	SelectObjectAction* selectObjectAction;

	std::vector<glm::ivec2> selectLasso;
	
	blib::wm::ToggleMenuItem* objectModeSnapToFloor;


	//gat
	int activeGatTile = 0;
	int newTextureSize = 4;

	class ShadowBrush
	{
	public:
		std::vector<std::vector<glm::vec3>> brush;
		std::string filename;
	};
	std::vector<ShadowBrush> shadowMapBrushes;
	int shadowMapBrush = 0;
	int shadowMapColor = 0;



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
	void heightEditUpdate();
	void detailHeightEditUpdate();
	void wallEditUpdate();
	void gatEditUpdate();
	void detailGatEditUpdate();
	void gatTypeEditUpdate();
	void colorEditUpdate();

	void finishObjectTransformAction(); // object edit
	void cancelObjectTransformAction(); // object edit
	void deleteSelectedObjects(); //object edit
	void duplicateSelectedObjects(); //object edit

	void setLightmap(float x, float y, int color, float blend);

	Rsw::Model* newModel;

	glm::ivec2 detailHeightCursor;
	glm::vec2 detailHeightOffset;




	v8::Isolate* isolate;
//	v8::Isolate::Scope* isolate_scope;
	//v8::HandleScope* handle_scope;

	v8::Handle<v8::Object> brow;

	std::list<JsRunner*> runNext;

	int version;
	void loadJsPlugins();
	void lightmapEditUpdate();


//menu
	void setCamera(std::function<Camera*()>, const std::string &name);
	void loadMap(std::string fileName, bool threaded = true);
	void saveMap(std::string fileName);

	void menuFileSave();
	void menuFileSaveAs();
	void menuFileSaveHeightmap();
	void menuFileLoadHeightmap();
	void menuFileExportObj();

	void menuActionsLightmapCalculate();
	void menuActionsLightmapSmooth();
	void menuActionsLightmapUnique();

	void menuActionsScaleDown();
};

