#include "ObjectWindow.h"
#include "../BrowEdit.h"
#include "../Camera.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/list.h>
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
		int index = getComponent<blib::wm::widgets::List>("lstObjects")->selectedItem;
		browEdit->camera->position = glm::vec2(
			5 * browEdit->map->getGnd()->width + browEdit->map->getRsw()->objects[index]->position.x, 
			5 * browEdit->map->getGnd()->height + browEdit->map->getRsw()->objects[index]->position.z);
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
