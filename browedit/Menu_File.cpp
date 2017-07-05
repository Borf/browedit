#include "BrowEdit.h"

#include "windows/MessageWindow.h"

#include <BroLib/Map.h>

#include <blib/BackgroundTask.h>
#include <blib/Util.h>
#include <blib/Window.h>

#include <direct.h>


void BrowEdit::menuFileSave()
{
	MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
	new blib::BackgroundTask<bool>(this, [this]()
	{
		if (map)
		{
			char prevDir[1024];
			_getcwd(prevDir, 1024);
			_chdir(blib::util::replace(config["data"]["ropath"].get<std::string>(), "/", "\\").c_str());
			map->save(map->getFileName());
			_chdir(prevDir);
		}
		return true;
	}, [dialog](bool bla)
	{
		dialog->close();
	});
}


void BrowEdit::menuFileSaveAs()
{
	char fileName[1024];
	strcpy(fileName, blib::util::replace(map->getFileName(), "/", "\\").c_str());

	char curdir[100];
	_getcwd(curdir, 100);
	HWND hWnd = this->window->hWnd;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 1024;
	ofn.lpstrFilter = "All\0*.*\0RO maps\0*.rsw\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn))
	{
		std::string newFileName(fileName);
		newFileName = blib::util::replace(newFileName, "/", "\\");
		if (newFileName.rfind(".") > newFileName.rfind("\\"))
			newFileName = newFileName.substr(0, newFileName.rfind("."));
		_chdir(curdir);


		MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
		new blib::BackgroundTask<bool>(this, [this, newFileName]()
		{
			if (map)
				map->save(newFileName);
			return true;
		}, [dialog](bool bla)
		{
			dialog->close();
		});
	}
	_chdir(curdir);
}






void BrowEdit::menuFileSaveHeightmap()
{
	MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
	new blib::BackgroundTask<bool>(this, [this]()
	{
		if (map)
		{
			char prevDir[1024];
			_getcwd(prevDir, 1024);
			_chdir(blib::util::replace(config["data"]["ropath"].get<std::string>(), "/", "\\").c_str());
			map->saveHeightmap(map->getFileName() + ".height.png");
			_chdir(prevDir);
		}
		return true;
	}, [dialog](bool bla) {	dialog->close();	});
}


void BrowEdit::menuFileLoadHeightmap()
{
	MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
	new blib::BackgroundTask<bool>(this, [this]()
	{
		if (map)
			map->loadHeightmap(config["data"]["ropath"].get<std::string>() + "/" + map->getFileName() + ".height.png");
		return true;
	}, [dialog](bool bla) {	dialog->close();	});
}


void BrowEdit::menuFileExportObj()
{
	MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
	new blib::BackgroundTask<bool>(this, [this]()
	{
		if (map)
			map->exportObj(config["data"]["ropath"].get<std::string>() + "/" + map->getFileName() + ".obj");
		return true;
	}, [dialog](bool bla) {	dialog->close();	});
}
