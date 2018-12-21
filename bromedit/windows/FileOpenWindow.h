#pragma once

#include <vector>

#include <blib/wm/widgets/list.h>
#include <blib/wm/Window.h>


class BromEdit;

class FileOpenWindow : public blib::wm::Window
{
	blib::wm::widgets::List* lstFiles;
	std::vector<std::string> files;
	BromEdit* bromEdit;
public:
	FileOpenWindow(blib::ResourceManager* resourceManager, BromEdit* browEdit);

	void open();
	bool listClick(int, int, int);
	bool filter(blib::Key key);
};