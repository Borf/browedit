#pragma once

#include <blib/wm/Window.h>


class BrowEdit;

class HelpWindow : public blib::wm::Window
{
public:
	HelpWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);


};