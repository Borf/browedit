#include "ObjectWindow.h"
#include "ModelPropertiesWindow.h"
#include "EffectPropertiesWindow.h"
#include "LightPropertiesWindow.h"
#include "../BrowEdit.h"
#include "../Camera.h"

#include <blib/wm/WM.h>
#include <blib/wm/widgets/TreeView.h>
#include <blib/wm/widgets/button.h>
#include <blib/wm/widgets/Label.h>
#include <blib/wm/widgets/List.h>
#include <blib/wm/widgets/ScrollPanel.h>
#include <blib/SpriteBatch.h>
#include <blib/util/FileSystem.h>
#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/Renderer.h>
#include <blib/FBO.h>
#include <blib/util/Log.h>
#include <blib/util/Profiler.h>
#include <glm/glm.hpp>

#include <BroLib/Map.h>
#include <BroLib/Rsw.h>
#include <BroLib/Gnd.h>
#include <BroLib/Rsm.h>
#include <BroLib/MapRenderer.h>

using blib::util::Log;




ObjectWindow::ObjectWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : blib::wm::Window("Objects", "ObjectWindow.json", resourceManager)
{
	this->browEdit = browEdit;
	this->resourceManager = resourceManager;
	x = 1000;
	y = 10;
	textureSize = 128;
	largeWidth = glm::min(1300, blib::wm::WM::getInstance()->screenSize.x - 100);
	resizable = false;

	getComponent<blib::wm::widgets::TreeView>("lstObjects")->addClickHandler([this, browEdit, resourceManager](int x, int y, int clickCount)
	{
		blib::wm::widgets::TreeView* treeView = getComponent<blib::wm::widgets::TreeView>("lstObjects");
		if (treeView->selectedItem >= (int)treeView->currentList.size())
			return true;
		if (!browEdit->map)
			return true;
		ObjectTreeNode* node = dynamic_cast<ObjectTreeNode*>(treeView->currentList[treeView->selectedItem].second);
		if (!node)
			return false;
		browEdit->camera->targetPosition = glm::vec2(
			5 * browEdit->map->getGnd()->width + node->object->position.x,
			5 * browEdit->map->getGnd()->height - node->object->position.z);

		for (size_t i = 0; i < browEdit->map->getRsw()->objects.size(); i++)
			browEdit->map->getRsw()->objects[i]->selected = node->object == browEdit->map->getRsw()->objects[i];

		if (clickCount == 2)
		{
			if (node->object->type == Rsw::Object::Type::Model)
				new ModelPropertiesWindow((Rsw::Model*)node->object, resourceManager, browEdit);
			if (node->object->type == Rsw::Object::Type::Effect)
				new EffectPropertiesWindow((Rsw::Effect*)node->object, resourceManager, browEdit);
			if (node->object->type == Rsw::Object::Type::Light)
				new LightPropertiesWindow(dynamic_cast<Rsw::Light*>(node->object), resourceManager, browEdit);


		}

		return true;
	});


	resizable = false;

	getComponent("btnExpand")->addClickHandler([this](int x, int y, int clickCount)
	{
		blib::wm::widgets::Button* expandButton = getComponent<blib::wm::widgets::Button>("btnExpand");

		if (expandButton->text == ">")
		{
			smallWidth = rootPanel->width;
			if (this->x + this->getWidth() > blib::wm::WM::getInstance()->screenSize.x / 2)
				setPosition(this->x - (largeWidth - smallWidth), this->y);
			setSize(largeWidth, rootPanel->height);
			expandButton->text = "<";
			resizable = true;
			arrangeComponents(smallWidth, height);

		}
		else
		{
			largeWidth = rootPanel->width;
			if (this->x + this->getWidth() > blib::wm::WM::getInstance()->screenSize.x / 2)
				setPosition(this->x + (largeWidth - smallWidth), this->y);

			setSize(smallWidth, rootPanel->height);
			expandButton->text = ">";

			resizable = false;
			arrangeComponents(largeWidth, height);
		}
		return true;
	});


	blib::util::StreamInFile* pFile = new blib::util::StreamInFile("assets/romodels.txt");

	std::map<std::string, bool> dirLookup;

	while (!pFile->eof())
	{
		std::string line = pFile->readLine();
		std::string dir = line.substr(0, line.find("|"));
		textureFiles[dir] = line.substr(line.find("|") + 1);
		dir = dir.substr(0, dir.rfind("/"));
		dirLookup[dir] = true;
	}
	delete pFile;

	std::vector<std::string> dirs;
	for (auto d : dirLookup)
		dirs.push_back(d.first);

	blib::wm::widgets::TreeView* tree = getComponent<blib::wm::widgets::TreeView>("lstFolders");


	tree->root = new blib::wm::widgets::TreeView::TreeNode();
	for (auto d : dirs)
	{
		blib::util::trim(d);
		std::vector<std::string> folders = blib::util::split(d, "/");
		if (folders.size() == 1)
			continue;
		blib::wm::widgets::TreeView::TreeNode* node = NULL;

		node = tree->root;
		for (size_t i = 0; i < folders.size() - 1; i++)
		{
			blib::wm::widgets::TreeView::TreeNode* child = node->getNode(folders[i]);
			if (!child)
				child = new blib::wm::widgets::TreeView::TreeNode(folders[i], node);
			node = child;
		}
		new blib::wm::widgets::TreeView::TreeNode(folders[folders.size() - 1], node);
	}
	tree->buildList();

	tree->addClickHandler([this, tree](int, int, int) {
		if (tree->selectedItem >= 0 && tree->selectedItem < (int)tree->currentList.size())
		{
			blib::wm::widgets::TreeView::TreeNode* n = tree->currentList[tree->selectedItem].second;
			std::string dir = "";
			while (n)
			{
				dir = n->text + "/" + dir;
				n = n->parent;
			}
			dir = dir.substr(1);
			setDirectory(dir);
		}
		return true;
	});


	getComponent<blib::wm::widgets::TreeView>("lstObjects")->root = new blib::wm::widgets::TreeView::TreeNode();
	modelsNode = new blib::wm::widgets::TreeView::TreeNode("Models", getComponent<blib::wm::widgets::TreeView>("lstObjects")->root);
	lightsNode = new blib::wm::widgets::TreeView::TreeNode("Lights", getComponent<blib::wm::widgets::TreeView>("lstObjects")->root);
	effectsNode = new blib::wm::widgets::TreeView::TreeNode("Effects", getComponent<blib::wm::widgets::TreeView>("lstObjects")->root);
	soundsNode = new blib::wm::widgets::TreeView::TreeNode("Sounds", getComponent<blib::wm::widgets::TreeView>("lstObjects")->root);
	getComponent<blib::wm::widgets::TreeView>("lstObjects")->buildList();


	setDirectory("/");

}

ObjectWindow::~ObjectWindow()
{

}


void ObjectWindow::arrangeComponents(int oldWidth, int oldHeight)
{
	blib::wm::Window::arrangeComponents(oldWidth, oldHeight);
}

void ObjectWindow::updateObjects(Map* map)
{
	blib::wm::widgets::TreeView* items = getComponent<blib::wm::widgets::TreeView>("lstObjects");


	blib::linq::deleteall(modelsNode->children);
	blib::linq::deleteall(effectsNode->children);
	blib::linq::deleteall(lightsNode->children);
	blib::linq::deleteall(soundsNode->children);


	for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
	{
		Rsw::Object* obj = map->getRsw()->objects[i];
		std::vector<std::string> folders = blib::util::split(obj->name, "\\");
		blib::wm::widgets::TreeView::TreeNode* node = NULL;
		
		if (obj->type == Rsw::Object::Type::Model)
			node = modelsNode;
		else if (obj->type == Rsw::Object::Type::Effect)
			node = effectsNode;
		else if (obj->type == Rsw::Object::Type::Light)
			node = lightsNode;
		else if (obj->type == Rsw::Object::Type::Sound)
			node = soundsNode;

		for (size_t i = 0; i < folders.size()-1; i++)
		{
			blib::wm::widgets::TreeView::TreeNode* child = node->getNode(folders[i]);
			if (!child)
				child = new blib::wm::widgets::TreeView::TreeNode(folders[i], node);
			node = child;
		}
		new ObjectTreeNode(folders[folders.size() - 1], obj, node);
	}
	items->buildList();
}





void ObjectWindow::setDirectory(const std::string &directory)
{
	blib::wm::widgets::ScrollPanel* panel = getComponent<blib::wm::widgets::ScrollPanel>("lstAllTextures");
	panel->clear();
	panel->internalWidth = panel->width;


	int px = 0;
	int py = 0;

	double start = blib::util::Profiler::getAppTime();
	for (auto it : textureFiles)
	{
		if (it.first.substr(0, directory.size()) == directory && it.first.find("/", directory.size() + 1) == -1)
		{
			ModelWidget* modelWidget = new ModelWidget(new Rsm(it.second), resourceManager, browEdit);
			modelWidget->width = textureSize;
			modelWidget->height = textureSize;
			modelWidget->x = px;
			modelWidget->y = py;
			panel->add(modelWidget);

			modelWidget->addClickHandler([this, it](int, int, int) {
				Log::out << it.second << Log::newline;
				std::for_each(browEdit->map->getRsw()->objects.begin(), browEdit->map->getRsw()->objects.end(), [](Rsw::Object* o) { o->selected = false; });
				browEdit->addModel(it.second.substr(11));
				getComponent<blib::wm::widgets::Button>("btnExpand")->onMouseClick(0, 0, 1);
				return true;
			});


			px += textureSize;

			if (px + textureSize > panel->width)
			{
				py += textureSize + 12;
				px = 0;
			}
		}
	}

	px = 0;
	py = 0;

	for (auto it : textureFiles)
	{
		if (it.first.substr(0, directory.size()) == directory && it.first.find("/", directory.size() + 1) == -1)
		{
			blib::wm::widgets::Label* label = new blib::wm::widgets::Label();
			label->text = it.first.substr(directory.size());
			label->width = textureSize;
			label->height = 12;
			label->x = px;
			label->y = py + textureSize;
			panel->add(label);
			px += textureSize;
			if (px + textureSize > panel->width)
			{
				py += textureSize + 12;
				px = 0;
			}
		}
	}


	Log::out << "Time taken: " << blib::util::Profiler::getAppTime() - start << Log::newline;
	panel->scrollX = 0;
	panel->scrollY = 0;
	panel->internalHeight = py + textureSize + 12;

}
























ModelWidget::ModelWidget(Rsm* rsm, blib::ResourceManager* resourceManager, BrowEdit* browedit)
{
	this->rsm = rsm;
	this->browedit = browedit;
	if (rsm->renderer == NULL)
	{
		rsm->renderer = new RsmModelRenderInfo();
		for (size_t i = 0; i < rsm->textures.size(); i++)
			rsm->renderer->textures.push_back(resourceManager->getResource<blib::Texture>("data/texture/" + rsm->textures[i]));
	}
	fbo = resourceManager->getResource<blib::FBO>();
	fbo->depth = true;
	rotation = 0;
}

int blablaindex = 0;

void ModelWidget::draw(blib::SpriteBatch &spriteBatch, glm::mat4 matrix, blib::Renderer* renderer) const
{
	if (width - 4 != fbo->width || height - 4 != fbo->height)
	{
		fbo->setSize(width - 4, height - 4);
		if (rsm->loaded)
			browedit->mapRenderer.renderMeshFbo(rsm, rotation, fbo, renderer);
	}

	if (rsm->loaded && this->hover)
	{
		const_cast<ModelWidget*>(this)->rotation++;	//ahem
		browedit->mapRenderer.renderMeshFbo(rsm, rotation, fbo, renderer);
	}

	spriteBatch.drawStretchyRect(blib::wm::WM::getInstance()->skinTexture, glm::translate(matrix, glm::vec3(x, y, 0)), blib::wm::WM::getInstance()->skin["list"], glm::vec2(width, height));

	if (rsm->loaded)
		spriteBatch.draw(fbo, glm::translate(matrix, glm::vec3(x + 2, y + 2, 0)));
}

ModelWidget::~ModelWidget()
{
	if (rsm)
		delete rsm;
	rsm = NULL;
	if (fbo)
		fbo->free();
}
