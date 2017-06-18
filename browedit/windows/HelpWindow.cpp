#include "HelpWindow.h"
#include "../BrowEdit.h"


#include <blib/wm/widgets/list.h>
#include <blib/wm/widgets/BmlBox.h>
#include <blib/util/FileSystem.h>


HelpWindow::HelpWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : Window("Help", "HelpWindow.json", resourceManager)
{
	json help = blib::util::FileSystem::getJson("assets/help.json");

	blib::wm::widgets::List* list = getComponent<blib::wm::widgets::List>("lstTopics");

	for (size_t i = 0; i < help.size(); i++)
	{
		list->items.push_back(help[i]["title"].get<std::string>());
	}


	list->addClickHandler([help, list, this](int x, int y, int clickCount)
	{
		if (list->selectedItems.empty())
			return false;

		getComponent<blib::wm::widgets::BmlBox>("text")->setBml(help[list->selectedItems[0]]["value"].get<std::string>());
		return true;
	});

}

