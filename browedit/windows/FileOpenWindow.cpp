#include "FileOpenWindow.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/util/FileSystem.h>
#include <blib/util/FastDelegate.h>
#include <algorithm>



FileOpenWindow::FileOpenWindow(blib::ResourceManager* resourceManager) : blib::wm::Window("Open File", "OpenWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);

	lstFiles = getComponent<blib::wm::widgets::List>("lstFiles");


	std::vector<std::string> tmpFiles = blib::util::FileSystem::getFileList("data/");
	std::sort(tmpFiles.begin(), tmpFiles.end());

	for (size_t i = 0; i < tmpFiles.size(); i++)
	{
		std::transform(tmpFiles[i].begin(), tmpFiles[i].end(), tmpFiles[i].begin(), ::tolower);
		if (tmpFiles[i].substr(tmpFiles[i].length() - 4, 4) == ".rsw")
		{
			std::string name = tmpFiles[i].substr(5, tmpFiles[i].length() - 5 - 4);
			lstFiles->items.push_back(name);
			files.push_back(name);
		}
	}
//	lstFiles->addClickHandler(fastdelegate::MakeDelegate(this, &FileOpenWindow::btnOpenClick));

	getComponent("txtFilter")->addKeyHandler(fastdelegate::MakeDelegate(this, &FileOpenWindow::filter));
//	getComponent("btnOpen")->addClickHandler(fastdelegate::MakeDelegate(this, &FileOpenWindow::btnOpenClick));
	selectedWidget = getComponent("txtFilter");
	selectedWidget->selected = true;


}

void FileOpenWindow::filter(char key)
{
	if (key == '\r' || key == '\n')
	{
		btnOpenClick(0, 0);
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
	lstFiles->selectedItem = lstFiles->items.size() == 0 ? -1 : 0;
}

void FileOpenWindow::btnOpenClick(int x, int y)
{
//	if (lstFiles->selectedItem != -1)
//		BrowEdit::getInstance()->initWorld(lstFiles->items[lstFiles->selectedItem]);
	close();
}
