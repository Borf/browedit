#pragma once

#include <blib/wm/Window.h>


class BrowEdit;

class NewMapWindow : public blib::wm::Window
{
public:
	NewMapWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);


};