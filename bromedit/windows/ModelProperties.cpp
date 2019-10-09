#include "ModelProperties.h"

#include <BroLib/MapRenderer.h>
#include <blib/ResourceManager.h>
#include <blib/wm/widgets/textbox.h>
#include <blib/wm/widgets/list.h>
#include <blib/wm/widgets/button.h>


ModelProperties::ModelProperties(blib::ResourceManager * resourceManager) : blib::wm::Window("Model Properties", "RsmModelPropertiesWindow.json", resourceManager)
{
	shadeType = getComponent<blib::wm::widgets::Textbox>("shadetype");
	texture = getComponent<blib::wm::widgets::Textbox>("texture");
	textures = getComponent<blib::wm::widgets::List>("textures");
	textures->multiselect = false;

	btnAdd = getComponent<blib::wm::widgets::Button>("btnAdd");
	btnEdit = getComponent<blib::wm::widgets::Button>("btnEdit");
	btnDel = getComponent<blib::wm::widgets::Button>("btnDelete");

	textures->addClickHandler([this](int x, int y, int clickCount)
	{
		if (textures->selectedItems.empty())
			return true;
		texture->text = textures->items[textures->selectedItems[0]];
		return true;
	});

	btnAdd->addClickHandler([this, resourceManager](int x, int y, int clickCount)
	{
		textures->items.push_back(texture->text);
		selectedModel->renderer->textures.push_back(resourceManager->getResource<blib::Texture>("data/texture/" + texture->text));
		selectedModel->textures.push_back(texture->text);
		return true;
	});

	btnEdit->addClickHandler([this, resourceManager](int x, int y, int clickCount)
	{
		if (textures->selectedItems.empty())
			return true;

		textures->items[textures->selectedItems[0]] = texture->text;
		resourceManager->dispose(selectedModel->renderer->textures[textures->selectedItems[0]]);
		selectedModel->renderer->textures[textures->selectedItems[0]] = resourceManager->getResource<blib::Texture>("data/texture/" + texture->text);
		selectedModel->textures[textures->selectedItems[0]] = texture->text;
		return true;
	});


	btnDel->addClickHandler([this, resourceManager](int x, int y, int clickCount)
	{
		if (textures->selectedItems.empty() || textures->items.size() == 1)
			return true;

		resourceManager->dispose(selectedModel->renderer->textures[textures->selectedItems[0]]);
		selectedModel->renderer->textures.erase(selectedModel->renderer->textures.begin() + textures->selectedItems[0]);
		selectedModel->textures.erase(selectedModel->textures.begin() + textures->selectedItems[0]);
		textures->items.erase(textures->items.begin() + textures->selectedItems[0]);
		return true;
	});

}



void ModelProperties::selectModel(IRsm* model)
{
	selectedModel = model;
	shadeType->text = std::to_string(model->shadeType);

	texture->text = "";
	textures->items.clear();

	for (auto & t : model->textures)
		textures->items.push_back(t);

}
