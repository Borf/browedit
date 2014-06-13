#pragma once

#include <blib/wm/Window.h>
#include <blib/wm/widgets/Image.h>

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <map>



class Map;
namespace blib { class Texture; }

class TextureWindow;
class BrowEdit;

class SelectableImage : public blib::wm::widgets::Image
{
	int selectX1;
	int selectY1;

	int selectX2;
	int selectY2;

	int gridX;
	int gridY;

	int index;
	bool dragging;
	TextureWindow* textureWindow;
public:
	SelectableImage(blib::Texture* texture, int index, TextureWindow* textureWindow);
	virtual void draw(blib::SpriteBatch &shader, glm::mat4 matrix, blib::Renderer* renderer) const;
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

	blib::ResourceManager* resourceManager;

	std::vector<SelectableImage*> images;
	BrowEdit* browEdit;


	std::map<std::string, std::string> textureFiles;



	int textureSize;

public:
	TextureWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);
	~TextureWindow();

	void updateTextures(Map* map);
	SelectableImage* getImage();
	void setActiveTexture(int index);
	void setDirectory(std::string directory);

	virtual void arrangeComponents(int oldWidth, int oldHeight);

	int selectedImage;
	glm::vec2 tx1;
	glm::vec2 tx2;
};

