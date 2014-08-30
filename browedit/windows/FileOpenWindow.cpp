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
	getComponent("txtFilter")->addKeyHandler(std::bind(&FileOpenWindow::filter, this, std::placeholders::_1));
	lstFiles->addClickHandler(std::bind(&FileOpenWindow::listClick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	getComponent("btnOpen")->addClickHandler(std::bind(&FileOpenWindow::btnOpenClick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	selectedWidget = getComponent("txtFilter");
	selectedWidget->selected = true;


	getComponent("btnCancel")->addClickHandler([this](blib::wm::Widget*, int,int,int) { close(); });

	filter('\0');

}

void FileOpenWindow::filter(char key)
{
	if (key == '\r' || key == '\n')
	{
		btnOpenClick(NULL, 0, 0,1);
		return;
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
}


void FileOpenWindow::listClick(blib::wm::Widget*, int, int, int clickCount)
{
	if (clickCount == 2)
		btnOpenClick(NULL, 0, 0, 0);
}

void FileOpenWindow::btnOpenClick(blib::wm::Widget*, int, int, int)
{

	if (lstFiles->selectedItem() == -1)
		return;

	std::string fileName = "data\\" + lstFiles->items[lstFiles->selectedItem()];

	browEdit->loadMap(fileName);
	close();
}
