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

class MeshProperties;

class BromEdit : public blib::App, public blib::MouseListener
{
	json config;
	json translation;

	blib::MouseState lastMouseState;
	blib::MouseState startMouseState;
	blib::KeyState lastKeyState;

	blib::Font* font;

	glm::vec2 rotation;
	float distance = 0;

	Rsm::Mesh* selectedMesh = nullptr;


	blib::Texture* grid;

public:
	BromEdit(const json &config);
	~BromEdit(void);

	virtual void init();
	virtual void update(double elapsedTime);
	virtual void draw();


	Rsm* model;
	RsmModelRenderInfo* renderInfo;

	MapRenderer mapRenderer;
	blib::wm::WM* wm;

	MeshProperties* meshProperties;

};

