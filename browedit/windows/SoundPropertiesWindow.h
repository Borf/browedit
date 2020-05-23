#pragma once

#include <BroLib/Rsw.h>
#include <blib/wm/Window.h>


class BrowEdit;
namespace blib { class ResourceManager; }

class SoundPropertiesWindow : public blib::wm::Window
{
public:
	SoundPropertiesWindow(Rsw::Sound* effect, blib::ResourceManager* resourceManager, BrowEdit* browEdit);
};