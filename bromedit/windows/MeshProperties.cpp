#include "MeshProperties.h"

#include <BroLib/MapRenderer.h>
#include <blib/ResourceManager.h>
#include <blib/wm/widgets/textbox.h>

MeshProperties::MeshProperties(blib::ResourceManager * resourceManager) : blib::wm::Window("Mesh Properties", "MeshPropertiesWindow.json", resourceManager)
{
	addHandler("name",			[](Rsm::Mesh* mesh) { return &mesh->name; });
	addHandler("position1x",	[](Rsm::Mesh* mesh) { return &mesh->pos[0]; });
	addHandler("position1y",	[](Rsm::Mesh* mesh) { return &mesh->pos[1]; });
	addHandler("position1z",	[](Rsm::Mesh* mesh) { return &mesh->pos[2]; });
	addHandler("position2x",	[](Rsm::Mesh* mesh) { return &mesh->pos_[0]; });
	addHandler("position2y",	[](Rsm::Mesh* mesh) { return &mesh->pos_[1]; });
	addHandler("position2z",	[](Rsm::Mesh* mesh) { return &mesh->pos_[2]; });

	addHandler("scalex",		[](Rsm::Mesh* mesh) { return &mesh->scale[0]; });
	addHandler("scaley",		[](Rsm::Mesh* mesh) { return &mesh->scale[1]; });
	addHandler("scalez",		[](Rsm::Mesh* mesh) { return &mesh->scale[2]; });

	addHandler("rotation",		[](Rsm::Mesh* mesh) { return &mesh->rotangle; });
	addHandler("rotationx",		[](Rsm::Mesh* mesh) { return &mesh->rotaxis[0]; });
	addHandler("rotationy",		[](Rsm::Mesh* mesh) { return &mesh->rotaxis[1]; });
	addHandler("rotationz",		[](Rsm::Mesh* mesh) { return &mesh->rotaxis[2]; });

	addHandler("offset11",		[](Rsm::Mesh* mesh) { return &mesh->offset[0][0]; });
	addHandler("offset12",		[](Rsm::Mesh* mesh) { return &mesh->offset[0][1]; });
	addHandler("offset13",		[](Rsm::Mesh* mesh) { return &mesh->offset[0][2]; });
	addHandler("offset21",		[](Rsm::Mesh* mesh) { return &mesh->offset[1][0]; });
	addHandler("offset22",		[](Rsm::Mesh* mesh) { return &mesh->offset[1][1]; });
	addHandler("offset23",		[](Rsm::Mesh* mesh) { return &mesh->offset[1][2]; });
	addHandler("offset31",		[](Rsm::Mesh* mesh) { return &mesh->offset[2][0]; });
	addHandler("offset32",		[](Rsm::Mesh* mesh) { return &mesh->offset[2][1]; });
	addHandler("offset33",		[](Rsm::Mesh* mesh) { return &mesh->offset[2][2]; });

}



void MeshProperties::selectMesh(IRsm::IMesh * mesh)
{
	selectedMesh = (Rsm::Mesh*)mesh;

	for (auto& c : strCallbacks)
		c.first->text = *c.second(selectedMesh);

	for (auto& c : floatCallbacks)
		c.first->text = blib::util::toString(*c.second(selectedMesh));

}

void MeshProperties::addHandler(const std::string & name, std::function<std::string*(Rsm::Mesh*)> callback)
{
	auto textbox = this->getComponent<blib::wm::widgets::Textbox>(name);
	textbox->addChangeHandler([this,textbox, callback]()
	{
		*callback(selectedMesh) = textbox->text;
	});
	strCallbacks.push_back(std::pair<blib::wm::widgets::Textbox*, std::function<std::string*(Rsm::Mesh*)>>(textbox, callback));
}

void MeshProperties::addHandler(const std::string & name, std::function<float*(Rsm::Mesh*)> callback)
{
	auto textbox = this->getComponent<blib::wm::widgets::Textbox>(name);
	textbox->addChangeHandler([this, textbox, callback]()
	{
		if (selectedMesh != nullptr)
		{
			*callback(selectedMesh) = (float)atof(textbox->text.c_str());
			selectedMesh->matrixDirty = true;
			selectedMesh->model->updateMatrices();
		}
	});
	floatCallbacks.push_back(std::pair<blib::wm::widgets::Textbox*, std::function<float*(Rsm::Mesh*)>>(textbox, callback));
}
