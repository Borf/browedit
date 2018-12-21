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
		}
	}
}

class FrameProperties : public blib::wm::Window
{
	Rsm::Mesh::Frame* selectedFrame;
	std::vector< std::pair<blib::wm::widgets::Textbox*, std::function<int*(Rsm::Mesh::Frame*)>>> intCallbacks;
	std::vector< std::pair<blib::wm::widgets::Textbox*, std::function<float*(Rsm::Mesh::Frame*)>>> floatCallbacks;
public:



	FrameProperties(blib::ResourceManager* resourceManager);

	void selectFrame(Rsm::Mesh::Frame* frame);

	void addHandler(const std::string &name, std::function<int*(Rsm::Mesh::Frame*)> callback);
	void addHandler(const std::string &name, std::function<float*(Rsm::Mesh::Frame*)> callback);
};