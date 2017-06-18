#pragma once

#include <BroLib/Rsw.h>
#include <blib/wm/Window.h>


class BrowEdit;
namespace blib { class ResourceManager; }

class LightPropertiesWindow : public blib::wm::Window
{
public:
	LightPropertiesWindow(Rsw::Light* light, blib::ResourceManager* resourceManager, BrowEdit* browEdit);
};