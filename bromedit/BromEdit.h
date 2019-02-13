#pragma once

#include <blib/App.h>
#include <blib/MouseListener.h>
#include <BroLib/MapRenderer.h>
#include <blib/json.hpp>

namespace blib {
	class Texture;
	class Font;
	namespace wm { class WM; class Menu; class ToggleMenuItem; }
}

class ModelProperties;
class MeshProperties;
class FrameProperties;

class BromEdit : public blib::App, public blib::MouseListener
{

	blib::MouseState lastMouseState;
	blib::MouseState startMouseState;
	blib::KeyState lastKeyState;

	blib::Font* font;

	//camera
	glm::vec2 rotation;
	float distance = 0;

	Rsm::Mesh* selectedMesh = nullptr;
	float timeSelect = 0;




	blib::Texture* grid;
	blib::Texture* buttons;
	blib::Texture* whitePixel;

public:
	BromEdit(const json &config);
	~BromEdit(void);

	virtual void init();
	virtual void update(double elapsedTime);
	virtual void draw();
	void testStuff();

	json config;

	Rsm* model;
	RsmModelRenderInfo* renderInfo;

	MapRenderer mapRenderer;
	blib::wm::WM* wm;

	ModelProperties* modelProperties = nullptr;
	MeshProperties* meshProperties = nullptr;
	FrameProperties* frameProperties = nullptr;

	void loadModel(const std::string &fileName);


	void pause();
	void play();
	void addKeyframe();
	void delKeyframe();
	void prevFrame();
	void nextFrame();
	void addMesh();
	void delMesh();

	void replaceMesh();
	void exportMesh();


	void menuFileNew();
	void menuFileSave();
	void menuFileSaveAs();
};

