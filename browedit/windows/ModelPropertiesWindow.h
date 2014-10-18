#pragma once

#include <BroLib/Rsw.h>
#include <blib/wm/Window.h>


class BrowEdit;
namespace blib { class ResourceManager; }

class ModelPropertiesWindow : public blib::wm::Window
{
public:
	ModelPropertiesWindow(Rsw::Model* model, blib::ResourceManager* resourceManager, BrowEdit* browEdit);
};