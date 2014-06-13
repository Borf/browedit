#include "ObjectWindow.h"
#include "../BrowEdit.h"
#include "../Camera.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/list.h>
#include <blib/wm/widgets/button.h>
#include <blib/wm/widgets/Label.h>
#include <blib/wm/widgets/ScrollPanel.h>
#include <blib/SpriteBatch.h>
#include <blib/util/FileSystem.h>
#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/Renderer.h>
#include <blib/FBO.h>
#include <blib/util/Log.h>
#include <blib/util/Profiler.h>
#include <glm/glm.hpp>

#include <BroLib/Map.h>
#include <BroLib/Rsw.h>
#include <BroLib/Gnd.h>
#include <BroLib/Rsm.h>
#include <BroLib/MapRenderer.h>

class Rsm;

using blib::util::Log;


class ModelWidget : public blib::wm::Widget
{
	Rsm* rsm;
	BrowEdit* browedit;

	blib::FBO* fbo;
	float rotation;
public:
	ModelWidget(Rsm* rsm, blib::ResourceManager* resourceManager, BrowEdit* browedit)
	{
		this->rsm = rsm;
		this->browedit = browedit;
		if (rsm->renderer == NULL)
		{
			rsm->renderer = new RsmModelRenderInfo();
			for (size_t i = 0; i < rsm->textures.size(); i++)
				rsm->renderer->textures.push_back(resourceManager->getResource<blib::Texture>("data/texture/" + rsm->textures[i]));
		}
		fbo = resourceManager->getResource<blib::FBO>();
		fbo->depth = true;
		rotation = 0;
	}


	virtual void draw(blib::SpriteBatch &spriteBatch, glm::mat4 matrix, blib::Renderer* renderer) const
	{
		if (width != fbo->width || height != fbo->height)
		{
			fbo->setSize(width, height);
			if (rsm->loaded)
				browedit->mapRenderer.renderMeshFbo(rsm, rotation, fbo, renderer);
		}

		if (rsm->loaded && this->hover)
		{
			const_cast<ModelWidget*>(this)->rotation++;	//ahem
			browedit->mapRenderer.renderMeshFbo(rsm, rotation, fbo, renderer);
		}

		spriteBatch.drawStretchyRect(blib::wm::WM::getInstance()->skinTexture, glm::translate(matrix, glm::vec3(x, y, 0)), blib::wm::WM::getInstance()->skin["list"], glm::vec2(width, height));


		spriteBatch.draw(fbo, glm::translate(matrix, glm::vec3(x, y, 0)));

	}
};

ObjectWindow::ObjectWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Objects", "ObjectWindow.json", resourceManager)
{
	this->browEdit = browEdit;
	this->resourceManager = resourceManager;
	x = 1000;
	y = 10;
	textureSize = 128;
	largeWidth = glm::min(1300, blib::wm::WM::getInstance()->screenSize.x - 100);
	resizable = false;

	getComponent<blib::wm::widgets::List>("lstObjects")->addClickHandler([this, browEdit](blib::wm::Widget*, int, int, int)
	{
		int index = getComponent<blib::wm::widgets::List>("lstObjects")->selectedItem();
		if (index == -1)
			return;
		browEdit->camera->targetPosition = glm::vec2(
			5 * browEdit->map->getGnd()->width + browEdit->map->getRsw()->objects[index]->position.x, 
			5 * browEdit->map->getGnd()->height - browEdit->map->getRsw()->objects[index]->position.z);

		for (size_t i = 0; i < browEdit->map->getRsw()->objects.size(); i++)
			browEdit->map->getRsw()->objects[i]->selected = i == index;

	});


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


	blib::util::StreamInFile* pFile = new blib::util::StreamInFile("assets/romodels.txt");

	std::map<std::string, bool> dirLookup;

	while (!pFile->eof())
	{
		std::string line = pFile->readLine();
		std::string dir = line.substr(0, line.find("|"));
		textureFiles[dir] = line.substr(line.find("|") + 1);
		dir = dir.substr(0, dir.rfind("/"));
		dirLookup[dir] = true;
	}
	delete pFile;

	std::vector<std::string> dirs;
	for (auto d : dirLookup)
		dirs.push_back(d.first);
	getComponent<blib::wm::widgets::List>("lstFolders")->items = dirs;
	getComponent<blib::wm::widgets::List>("lstFolders")->addClickHandler([this](blib::wm::Widget*, int, int, int) {
		blib::wm::widgets::List* l = getComponent<blib::wm::widgets::List>("lstFolders");
		if (l->selectedItem() >= 0 && l->selectedItem() < l->items.size())
			setDirectory(l->items[l->selectedItem()] + "/");
	});


	/*blib::wm::widgets::ScrollPanel* panel = getComponent<blib::wm::widgets::ScrollPanel>("lstAllTextures");
	ModelWidget* widget = new ModelWidget(new Rsm("data\\model\\�����׶�\\�м���.rsm"), resourceManager, browEdit);
	widget->width = 128;
	widget->height = 128;
	widget->x = 10;
	widget->y = 10;
	panel->add(widget);
	*/

	setDirectory("/");

}

ObjectWindow::~ObjectWindow()
{

}


void ObjectWindow::arrangeComponents(int oldWidth, int oldHeight)
{
	blib::wm::Window::arrangeComponents(oldWidth, oldHeight);
}

void ObjectWindow::updateObjects(Map* map)
{
	blib::wm::widgets::List* items = getComponent<blib::wm::widgets::List>("lstObjects");
	items->items.clear();
	for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
	{
		items->items.push_back(map->getRsw()->objects[i]->name);
	}
}





void ObjectWindow::setDirectory(const std::string &directory)
{
	blib::wm::widgets::ScrollPanel* panel = getComponent<blib::wm::widgets::ScrollPanel>("lstAllTextures");
	panel->clear();
	panel->internalWidth = panel->width;


	int px = 0;
	int py = 0;

/*	if (directory != "/")
	{
		blib::wm::widgets::Image* image = new blib::wm::widgets::Image(resourceManager->getResource<blib::Texture>("assets/textures/folder.png"));
		image->width = textureSize;
		image->height = textureSize;
		image->x = px;
		image->y = py;
		panel->add(image);

		px += textureSize;

		if (px + textureSize > panel->width)
		{
			py += textureSize + 12;
			px = 0;
		}
	}*/

	double start = blib::util::Profiler::getAppTime();
	for (auto it : textureFiles)
	{
		if (it.first.substr(0, directory.size()) == directory && it.first.find("/", directory.size() + 1) == -1)
		{
			ModelWidget* image = new ModelWidget(new Rsm(it.second), resourceManager, browEdit);
			image->width = textureSize;
			image->height = textureSize;
			image->x = px;
			image->y = py;
			panel->add(image);

			px += textureSize;

			if (px + textureSize > panel->width)
			{
				py += textureSize + 12;
				px = 0;
			}
		}
	}

	px = 0;
	py = 0;

	/*blib::wm::widgets::Label* label = new blib::wm::widgets::Label();
	label->text = "Up";
	label->width = textureSize;
	label->height = 12;
	label->x = px;
	label->y = py + 100;
	panel->add(label);
	px += textureSize;
	if (px + textureSize > panel->width)
	{
		py += textureSize + 12;
		px = 0;
	}*/


	for (auto it : textureFiles)
	{
		if (it.first.substr(0, directory.size()) == directory && it.first.find("/", directory.size() + 1) == -1)
		{
			blib::wm::widgets::Label* label = new blib::wm::widgets::Label();
			label->text = it.first.substr(directory.size());
			label->width = textureSize;
			label->height = 12;
			label->x = px;
			label->y = py + textureSize;
			panel->add(label);
			px += textureSize;
			if (px + textureSize > panel->width)
			{
				py += textureSize + 12;
				px = 0;
			}
		}
	}


	Log::out << "Time taken: " << blib::util::Profiler::getAppTime() - start << Log::newline;
	panel->scrollX = 0;
	panel->scrollY = 0;
	panel->internalHeight = py + textureSize + 12;

}