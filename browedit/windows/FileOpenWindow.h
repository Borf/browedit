#pragma once

#include <list>

#include <blib/wm/widgets/list.h>
#include <blib/wm/Window.h>


class BrowEdit;

class FileOpenWindow : public blib::wm::Window
{
	blib::wm::widgets::List* lstFiles;
	std::list<std::string> files;
	BrowEdit* browEdit;
public:
	FileOpenWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);

	void open();
	bool listClick(int, int, int);
	bool filter(blib::Key key);
};