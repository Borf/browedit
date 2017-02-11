#pragma once

#include <BroLib/Rsw.h>
#include <blib/wm/Window.h>


class BrowEdit;
namespace blib { class ResourceManager; }

class EffectPropertiesWindow : public blib::wm::Window
{
public:
	EffectPropertiesWindow(Rsw::Effect* effect, blib::ResourceManager* resourceManager, BrowEdit* browEdit);
};