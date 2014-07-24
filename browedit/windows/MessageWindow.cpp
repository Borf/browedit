#include "MessageWindow.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/Label.h>
#include <algorithm>



MessageWindow::MessageWindow(blib::ResourceManager* resourceManager, std::string message, std::string title) : blib::wm::Window(title, "MessageWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);

	getComponent<blib::wm::widgets::Label>("message")->text = message;
}
