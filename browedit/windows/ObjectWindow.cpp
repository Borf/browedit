#include "ObjectWindow.h"
#include "../BrowEdit.h"
#include "../Camera.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/list.h>
#include <blib/wm/widgets/button.h>
#include <blib/util/FileSystem.h>
#include <glm/glm.hpp>

#include <BroLib/Map.h>
#include <BroLib/Rsw.h>
#include <BroLib/Gnd.h>

ObjectWindow::ObjectWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Objects", "ObjectWindow.json", resourceManager)
{
	this->browEdit = browEdit;
	this->resourceManager = resourceManager;
	x = 1000;
	y = 10;
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

}