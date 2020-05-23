#include "FileOpenWindow.h"
#include "../BrowEdit.h"
#include <BroLib/Map.h>

#include <blib/wm/WM.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/util/FileSystem.h>
#include <blib/Util.h>
#include <algorithm>

#include <direct.h>
#include <commdlg.h>


std::string getOpenFile(const char* defaultFilename, const char* filter)
{
#ifdef WIN32
	char curdir[100];
	_getcwd(curdir, 100);
	HWND hWnd = NULL;// ::GetActiveWindow();
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;

	char buf[256];
	ZeroMemory(buf, 256);
	strcpy(buf, defaultFilename);
	ofn.lpstrFile = buf;
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;
	if (GetOpenFileName(&ofn))
	{
		_chdir(curdir);
		return buf;
	}
	_chdir(curdir);
#endif
	return "";
}


FileOpenWindow::FileOpenWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Open File", "OpenWindow.json", resourceManager)
{
	this->browEdit = browEdit;
	blib::wm::WM::getInstance()->center(this);

	lstFiles = getComponent<blib::wm::widgets::List>("lstFiles");


	std::vector<std::string> tmpFiles = blib::util::FileSystem::getFileList("data/");
	std::sort(tmpFiles.begin(), tmpFiles.end());
	tmpFiles.erase(std::unique(tmpFiles.begin(), tmpFiles.end()), tmpFiles.end());
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


	getComponent("btnBrowse")->addClickHandler([this, browEdit](int, int, int) 
	{ 
		char curdir[100];
		_getcwd(curdir, 100);

		_chdir(blib::util::replace(browEdit->getConfig()["data"]["ropath"].get<std::string>(), "/", "\\").c_str());

		std::string fileName = browEdit->map->getFileName() + ".rsw";
		fileName = blib::util::replace(fileName, "/", "\\");

		fileName = getOpenFile(fileName.c_str(), "All\0*.*\0RO Maps\0*.rsw\0");
		if (fileName != "")
		{
			fileName = fileName.substr(0, fileName.rfind("."));
			browEdit->loadMap(fileName);
		}

		_chdir(curdir);
		close();
		return true; 
	
	});

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
	if (key == blib::Key::DOWN && !lstFiles->selectedItems.empty())
	{
		lstFiles->selectedItems = { glm::min(lstFiles->selectedItems[0] + 1, (int)lstFiles->items.size() - 1) };
		return true;
	}
	if (key == blib::Key::UP && !lstFiles->selectedItems.empty())
	{
		lstFiles->selectedItems = { glm::max(lstFiles->selectedItems[0] - 1, 0) };
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
