#include "FileOpenWindow.h"
#include "../BrowEdit.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/util/FileSystem.h>
#include <algorithm>



FileOpenWindow::FileOpenWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Open File", "OpenWindow.json", resourceManager)
{
	this->browEdit = browEdit;
	blib::wm::WM::getInstance()->center(this);

	lstFiles = getComponent<blib::wm::widgets::List>("lstFiles");


	std::vector<std::string> tmpFiles = blib::util::FileSystem::getFileList("data/");
	std::sort(tmpFiles.begin(), tmpFiles.end());

	for (size_t i = 0; i < tmpFiles.size(); i++)
	{
		if (tmpFiles[i].size() < 4)
			continue;
		std::transform(tmpFiles[i].begin(), tmpFiles[i].end(), tmpFiles[i].begin(), ::tolower);
		if (tmpFiles[i].substr(tmpFiles[i].length() - 4, 4) == ".rsw")
		{
			std::string name = tmpFiles[i].substr(5, tmpFiles[i].length() - 5 - 4);
			lstFiles->items.push_back(name);
			files.push_back(name);
		}
	}
	getComponent("txtFilter")->addKeyDownHandler(std::bind(&FileOpenWindow::filter, this, std::placeholders::_1));
	lstFiles->addClickHandler(std::bind(&FileOpenWindow::listClick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	getComponent("btnOpen")->addClickHandler([this](int, int, int) { open(); return true; });
	selectedWidget = getComponent("txtFilter");
	selectedWidget->selected = true;


	getComponent("btnCancel")->addClickHandler([this](int, int, int) { close(); return true; });

	filter((blib::Key)'\0');

}

bool FileOpenWindow::filter(blib::Key key)
{
	if (key == blib::Key::ENTER)
	{
		open();
		return true;
	}

	std::string filt = getComponent<blib::wm::widgets::Textbox>("txtFilter")->text;
	std::transform(filt.begin(), filt.end(), filt.begin(), ::tolower);
	lstFiles->items.clear();
	for (std::list<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		if (it->find(filt) != -1)
			lstFiles->items.push_back(*it);
	}
	lstFiles->selectedItems.clear();
	if (!lstFiles->items.empty())
		lstFiles->selectedItems.push_back(0);
	return false;
}


bool FileOpenWindow::listClick(int, int, int clickCount)
{
	if (clickCount == 2)
	{
		open();
		return true;
	}
	return false;
}

void FileOpenWindow::open()
{

	if (lstFiles->selectedItem() == -1)
		return;

	std::string fileName = "data\\" + lstFiles->items[lstFiles->selectedItem()];

	browEdit->loadMap(fileName);
	close();
}
