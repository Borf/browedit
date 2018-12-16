#pragma once

#include <blib/wm/Window.h>


class BrowEdit;

namespace blib
{
	namespace wm
	{
		namespace widgets
		{
			class ProgressBar;
		}
	}
}



class ProgressWindow : public blib::wm::Window
{
	blib::wm::widgets::ProgressBar* bar;
public:
	ProgressWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);

	void setProgress(float f);
};