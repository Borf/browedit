#include "HelpWindow.h"
#include "../BrowEdit.h"


#include <blib/wm/widgets/list.h>
#include <blib/wm/widgets/Label.h>
#include <blib/util/FileSystem.h>


HelpWindow::HelpWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : Window("Help", "HelpWindow.json", resourceManager)
{
	Json::Value help = blib::util::FileSystem::getJson("assets/help.json");

	blib::wm::widgets::List* list = getComponent<blib::wm::widgets::List>("lstTopics");

	for (int i = 0; i < help.size(); i++)
	{
		list->items.push_back(help[i]["title"].asString());
	}


	list->addClickHandler([help, list, this](int x, int y, int clickCount)
	{
		if (list->selectedItems.empty())
			return false;

		getComponent<blib::wm::widgets::Label>("text")->text = help[list->selectedItems[0]]["value"].asString();
		return true;
	});

}

