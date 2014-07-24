#pragma once

#include <list>

#include <blib/wm/widgets/list.h>
#include <blib/wm/Window.h>


class BrowEdit;

class MessageWindow : public blib::wm::Window
{
public:
	MessageWindow(blib::ResourceManager* resourceManager, std::string message, std::string title = "");

};