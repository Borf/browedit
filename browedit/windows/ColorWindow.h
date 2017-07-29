#pragma once

#include <blib/wm/Window.h>


class BrowEdit;

class ColorWindow : public blib::wm::Window
{
public:
	ColorWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);

	glm::vec3 color;
};