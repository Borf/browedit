#pragma once

#include <blib/wm/Window.h>
#include <blib/wm/widgets/Image.h>

#include <glm/glm.hpp>

#include <list>
#include <string>


class Map;
namespace blib { class Texture; }

class SelectableImage : public blib::wm::widgets::Image
{
	int selectX1;
	int selectY1;

	int selectX2;
	int selectY2;

	int gridX;
	int gridY;

	int index;
public:
	SelectableImage(blib::Texture* texture, int index);
//	virtual void draw(OverlayShader* shader);
	virtual void mousedown(int x, int y);
	virtual void mouseup(int x, int y);
	virtual void mousedrag(int x, int y);
	virtual void mouseclick(int x, int y);
	void alignToGrid();
	glm::vec2 topLeft();
	glm::vec2 bottomRight();

};


class TextureWindow : public blib::wm::Window
{
	int smallWidth;
	int largeWidth;

	std::vector<SelectableImage*> images;

public:
	TextureWindow(blib::ResourceManager* resourceManager);
	~TextureWindow();

	void updateTextures(Map* map);
	SelectableImage* getImage();

	void expandButton(int x, int y);

	int selectedImage;

};

