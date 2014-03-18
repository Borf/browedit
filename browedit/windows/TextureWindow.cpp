#include "TextureWindow.h"

#include <BroLib/MapRenderer.h>
#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include <blib/SpriteBatch.h>
#include <blib/wm/widgets/Panel.h>
#include <blib/wm/widgets/button.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/Image.h>
#include <blib/wm/widgets/ScrollPanel.h>
#include <blib/wm/WM.h>
#include <blib/util/Log.h>
#include <blib/util/FileSystem.h>
#include <blib/ResourceManager.h>


using blib::util::Log;

#include <cctype>
#include <algorithm>



TextureWindow::TextureWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : Window("Texture", "TextureWindow.json", resourceManager)
{
	this->browEdit = browEdit;
	this->resourceManager = resourceManager;
	x = 1000;
	y = 10;
	largeWidth = 1300;
	selectedImage = -1;
	resizable = false;

	getComponent("btnExpand")->addClickHandler([this](blib::wm::Widget* widget, int x, int y, int clickCount)
	{
		blib::wm::widgets::Button* expandButton = getComponent<blib::wm::widgets::Button>("btnExpand");

		if (expandButton->text == ">")
		{
			smallWidth = rootPanel->width;
			if (this->x + this->getWidth() > blib::wm::WM::getInstance()->screenSize.x / 2)
				setPosition(this->x - (largeWidth - smallWidth), this->y);
			setSize(largeWidth, rootPanel->height);
			expandButton->text = "<";
			resizable = true;
			arrangeComponents(smallWidth, height);

		}
		else
		{
			largeWidth = rootPanel->width;
			if (this->x + this->getWidth() > blib::wm::WM::getInstance()->screenSize.x / 2)
				setPosition(this->x + (largeWidth - smallWidth), this->y);

			setSize(smallWidth, rootPanel->height);
			expandButton->text = ">";

			resizable = false;
			arrangeComponents(largeWidth, height);
		}
	});


	blib::util::StreamInFile* pFile = new blib::util::StreamInFile("assets/rotextures.txt");

	while (!pFile->eof())
	{
		std::string line = pFile->readLine();
		textureFiles[line.substr(0, line.find("|"))] = line.substr(line.find("|") + 1);
	}
	delete pFile;


	setDirectory("RO/splen/");
}


void TextureWindow::updateTextures(Map* map)
{
	blib::wm::widgets::ScrollPanel* panel = getComponent<blib::wm::widgets::ScrollPanel>("lstTextures");
	for (size_t i = 0; i < map->getGnd()->textures.size(); i++)
	{
		SelectableImage* img = new SelectableImage(map->getGnd()->textures[i]->texture, i, this);
		img->width = 192;
		img->height = 192;
		img->x = 0;
		img->y = 200 * i;
		panel->add(img);
		images.push_back(img);
	}
	panel->internalHeight = 192 * (map->getGnd()->textures.size() + 1);
}


SelectableImage* TextureWindow::getImage()
{
	return images[selectedImage];
}

void TextureWindow::setActiveTexture(int index)
{
	selectedImage = index;
}

void TextureWindow::setDirectory(std::string directory)
{
	blib::wm::widgets::ScrollPanel* panel = getComponent<blib::wm::widgets::ScrollPanel>("lstAllTextures");
	panel->clear();
	panel->internalWidth = panel->width;


	int px = 0;
	int py = 0;

	for (auto it : textureFiles)
	{
		if (it.first.substr(0, directory.size()) == directory && it.first.find("/", directory.size() + 1) == -1)
		{
			blib::wm::widgets::Image* image = new blib::wm::widgets::Image(resourceManager->getResource<blib::Texture>("data/texture/" + it.second));
			image->width = 64;
			image->height = 64;
			image->x = px;
			image->y = py;
			panel->add(image);

			px += 64;

			if (px + 64 > panel->width)
			{
				py += 64;
				px = 0;
			}
		}
	}
	panel->internalHeight = py + 64;


}

void TextureWindow::arrangeComponents(int oldWidth, int oldHeight)
{
	Window::arrangeComponents(oldWidth, oldHeight);

	if (width != oldWidth || height != oldHeight)
	{

		int px = 0;
		int py = 0;

	//	setDirectory("RO/splen/");
		blib::wm::widgets::ScrollPanel* panel = getComponent<blib::wm::widgets::ScrollPanel>("lstAllTextures");
		for (std::list<blib::wm::Widget*>::iterator it = panel->children.begin(); it != panel->children.end(); it++)
		{
			(*it)->x = px;
			(*it)->y = py;

			px += 64;
			if (px + 64 > panel->width)
			{
				py += 64;
				px = 0;
			}
		}
		panel->internalHeight = py + 64;




	}
}









SelectableImage::SelectableImage(blib::Texture* texture, int index, TextureWindow* textureWindow) : Image(texture)
{
	this->textureWindow = textureWindow;
	this->index = index;
	selectX1 = 0;
	selectY1 = 0;
	selectX2 = 0;
	selectY2 = 0;

	gridX = 4;
	gridY = 4;
}

void SelectableImage::draw(blib::SpriteBatch &spriteBatch, glm::mat4 matrix)
{
	Image::draw(spriteBatch, matrix);

	if ((selectX1 == 0 && selectX2 == 0 && selectY1 == 0 && selectY2 == 0) || textureWindow->selectedImage != index)
		return;

	Json::Value skin = blib::wm::WM::getInstance()->skin["list"];
	blib::Texture* texture = blib::wm::WM::getInstance()->skinTexture;

	spriteBatch.drawStretchyRect(texture, glm::translate(matrix, glm::vec3(x + selectX1, y + selectY1, 0)), skin, glm::vec2(selectX2 - selectX1, selectY2 - selectY1), glm::vec4(1, 1, 1, 0.5f));
}

void SelectableImage::mousedown(int x, int y)
{
	selectX1 = x - this->x;
	selectY1 = y - this->y;
	selectX2 = x - this->x;
	selectY2 = y - this->y;
	alignToGrid();
	textureWindow->setActiveTexture(index);
}

void SelectableImage::mouseup(int x, int y)
{
	selectX2 = x - this->x;
	selectY2 = y - this->y;
	alignToGrid();
	textureWindow->setActiveTexture(index);
}

void SelectableImage::mousedrag(int x, int y)
{
	selectX2 = x - this->x;
	selectY2 = y - this->y;
	alignToGrid();
	textureWindow->setActiveTexture(index);
}

void SelectableImage::mouseclick(int x, int y)
{
	selectX1 = 0;
	selectY1 = 0;
	selectX2 = width;
	selectY2 = height;
	alignToGrid();
	textureWindow->setActiveTexture(index);
}

void SelectableImage::alignToGrid()
{
	float px = width / (float)gridX;
	float py = height / (float)gridY;
	selectX1 = (int)(glm::floor(selectX1 / px)*px);
	selectX2 = (int)(glm::ceil(selectX2 / px)*px);
	selectY1 = (int)(glm::floor(selectY1 / py)*py);
	selectY2 = (int)(glm::ceil(selectY2 / py)*py);
}

glm::vec2 SelectableImage::topLeft()
{
	return glm::vec2(selectX1 / (float)width, selectY1 / (float)height);
}

glm::vec2 SelectableImage::bottomRight()
{
	return glm::vec2(selectX2 / (float)width, selectY2 / (float)height);
}
