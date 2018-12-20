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
	blib::wm::widgets::Textbox* name;
	blib::wm::widgets::Textbox* unknown1;
	blib::wm::widgets::Textbox* unknown2;
	blib::wm::widgets::Textbox* position1[3];
	blib::wm::widgets::Textbox* position2[3];
	blib::wm::widgets::Textbox* scale[3];
	blib::wm::widgets::Textbox* rotation;
	blib::wm::widgets::Textbox* rotationaxis[3];
	blib::wm::widgets::Textbox* offset[3][3];


	MeshProperties(blib::ResourceManager* resourceManager);

	void selectMesh(Rsm::Mesh* mesh);

	void addHandler(const std::string &name, std::function<std::string*(Rsm::Mesh*)> callback);
	void addHandler(const std::string &name, std::function<float*(Rsm::Mesh*)> callback);
};