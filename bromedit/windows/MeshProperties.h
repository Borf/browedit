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

class MeshProperties : public blib::wm::Window
{
	Rsm::Mesh* selectedMesh;
	std::vector< std::pair<blib::wm::widgets::Textbox*, std::function<std::string*(Rsm::Mesh*)>>> strCallbacks;
	std::vector< std::pair<blib::wm::widgets::Textbox*, std::function<float*(Rsm::Mesh*)>>> floatCallbacks;
public:
	MeshProperties(blib::ResourceManager* resourceManager);

	void selectMesh(Rsm::Mesh* mesh);

	void addHandler(const std::string &name, std::function<std::string*(Rsm::Mesh*)> callback);
	void addHandler(const std::string &name, std::function<float*(Rsm::Mesh*)> callback);
};