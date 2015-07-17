#pragma once

#include <BroLib/Rsw.h>
#include <blib/wm/Window.h>


class BrowEdit;
namespace blib { class ResourceManager; }

class MapSettingsWindow : public blib::wm::Window
{
public:
	MapSettingsWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);
};