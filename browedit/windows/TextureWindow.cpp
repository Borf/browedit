#include "TextureWindow.h"

#include <BroLib/MapRenderer.h>
#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include <blib/wm/widgets/Panel.h>
#include <blib/wm/widgets/button.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/Image.h>
#include <blib/wm/widgets/ScrollPanel.h>
#include <cctype>
#include <algorithm>





TextureWindow::TextureWindow(blib::ResourceManager* resourceManager) : Window("Texture", "TextureWindow.json", resourceManager)
{
	x = 1000;
	y = 10;
	//getComponent("btnExpand")->addClickHandler(MakeDelegate(this, &TextureWindow::expandButton));
	largeWidth = 1300;
	selectedImage = -1;
}


void TextureWindow::updateTextures(Map* map)
{
	blib::wm::widgets::ScrollPanel* panel = getComponent<blib::wm::widgets::ScrollPanel>("lstTextures");
	for (size_t i = 0; i < map->getGnd()->textures.size(); i++)
	{
		SelectableImage* img = new SelectableImage(map->getGnd()->textures[i]->texture, i);
		img->width = 192;
		img->height = 192;
		img->x = 0;
		img->y = 200 * i;
		panel->add(img);
		images.push_back(img);
	}
}



void TextureWindow::expandButton(int x, int y)
{
	blib::wm::widgets::Button* expandButton = getComponent<blib::wm::widgets::Button>("btnExpand");

	if (expandButton->text == ">")
	{
		smallWidth = rootPanel->width;
		setSize(largeWidth, rootPanel->height);
		expandButton->text = "<";
		resizable = true;
	}
	else
	{
		largeWidth = rootPanel->width;
		setSize(smallWidth, rootPanel->height);
		expandButton->text = ">";
		resizable = false;
	}
}

SelectableImage* TextureWindow::getImage()
{
	return images[selectedImage];
}
















SelectableImage::SelectableImage(blib::Texture* texture, int index) : Image(texture)
{
	this->index = index;
	selectX1 = 0;
	selectY1 = 0;
	selectX2 = 0;
	selectY2 = 0;

	gridX = 4;
	gridY = 4;
}

/*void SelectableImage::draw(OverlayShader* shader)
{
	Image::draw(shader);
	shader->setColor(glm::vec4(1, 1, 1, 0.25));
	shader->useTexture(false);
	glBegin(GL_QUADS);
	glVertexAttrib3f(0, (float)(x + selectX1), (float)(y + selectY1), 0);
	glVertexAttrib3f(0, (float)(x + selectX2), (float)(y + selectY1), 0);
	glVertexAttrib3f(0, (float)(x + selectX2), (float)(y + selectY2), 0);
	glVertexAttrib3f(0, (float)(x + selectX1), (float)(y + selectY2), 0);
	glEnd();

	if (BrowEdit::getInstance()->activeTexture == texture)
		shader->setColor(glm::vec4(0.5f, 1, 0.5f, 1));
	else
		shader->setColor(glm::vec4(1, 0, 0, 1));
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glVertexAttrib3f(0, (float)(x + selectX1), (float)(y + selectY1), 0);
	glVertexAttrib3f(0, (float)(x + selectX2), (float)(y + selectY1), 0);
	glVertexAttrib3f(0, (float)(x + selectX2), (float)(y + selectY2), 0);
	glVertexAttrib3f(0, (float)(x + selectX1), (float)(y + selectY2), 0);
	glEnd();
	glLineWidth(1);
	shader->setColor(glm::vec4(1, 1, 1, 1));
}
*/
void SelectableImage::mousedown(int x, int y)
{
	selectX1 = x - this->x;
	selectY1 = y - this->y;
	selectX2 = x - this->x;
	selectY2 = y - this->y;
	alignToGrid();
//	BrowEdit::getInstance()->activeTexture = texture;
}

void SelectableImage::mouseup(int x, int y)
{
	selectX2 = x - this->x;
	selectY2 = y - this->y;
	alignToGrid();
//	BrowEdit::getInstance()->activeTexture = texture;
//	BrowEdit::getInstance()->textureWindow->selectedImage = index;
}

void SelectableImage::mousedrag(int x, int y)
{
	selectX2 = x - this->x;
	selectY2 = y - this->y;
	alignToGrid();
//	BrowEdit::getInstance()->activeTexture = texture;
}

void SelectableImage::mouseclick(int x, int y)
{
	selectX1 = 0;
	selectY1 = 0;
	selectX2 = width;
	selectY2 = height;
	alignToGrid();
//	BrowEdit::getInstance()->activeTexture = texture;
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
