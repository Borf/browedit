#include "ProgressWindow.h"
#include "../BrowEdit.h"


#include <blib/wm/widgets/list.h>
#include <blib/wm/widgets/ProgressBar.h>
#include <blib/util/FileSystem.h>


ProgressWindow::ProgressWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : Window("Progress", "ProgressWindow.json", resourceManager)
{
	center();
	bar = getComponent<blib::wm::widgets::ProgressBar>("bar");
}


void ProgressWindow::setProgress(float value)
{
	bar->value = value;
}