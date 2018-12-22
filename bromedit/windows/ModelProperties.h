#pragma once

#include <blib/wm/Window.h>
#include <BroLib/Rsm.h>

namespace blib
{
	class ResourceManager;
	namespace wm
	{
		namespace widgets
		{
			class Textbox;
			class List;
			class Button;
		}
	}
}

class ModelProperties : public blib::wm::Window
{
	Rsm* selectedModel;

	blib::wm::widgets::Textbox* shadeType;
	blib::wm::widgets::Textbox* texture;
	blib::wm::widgets::List* textures;
	blib::wm::widgets::Button* btnAdd;
	blib::wm::widgets::Button* btnEdit;
	blib::wm::widgets::Button* btnDel;
public:
	ModelProperties(blib::ResourceManager* resourceManager);

	void selectModel(Rsm* mesh);
};