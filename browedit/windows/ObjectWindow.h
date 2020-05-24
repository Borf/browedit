#pragma once


#include <blib/wm/Window.h>
#include <blib/wm/widgets/Image.h>
#include <blib/wm/widgets/TreeView.h>
#include <blib/TextureMap.h>

#include <BroLib/Rsw.h>

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <map>

class BrowEdit;
class Map;
class IRsm;

namespace blib { class FBO; };

class ModelWidget : public blib::wm::Widget
{
	IRsm* rsm;
	BrowEdit* browedit;

	blib::FBO* fbo;
	float rotation;
public:
	ModelWidget(IRsm* rsm, blib::ResourceManager* resourceManager, BrowEdit* browedit);
	~ModelWidget();
	virtual void draw(blib::SpriteBatch &spriteBatch, glm::mat4 matrix, blib::Renderer* renderer) const;
};


class ObjectWindow : public blib::wm::Window
{
	static blib::Texture* lightTexture;
	static blib::Texture* effectTexture;
	static blib::TextureMap* effectTextureMap;
	static std::map<int, blib::TextureMap::TexInfo*> effectTextureInfo;


	class ObjectTreeNode : public blib::wm::widgets::TreeView::TreeNode
	{
	public:
		Rsw::Object* object;

		ObjectTreeNode(std::string name, Rsw::Object* object, blib::wm::widgets::TreeView::TreeNode* parent) : blib::wm::widgets::TreeView::TreeNode(name, parent)
		{
			this->object = object;
		}
	};


	void loadModelList();

	int smallWidth;
	int largeWidth;

	int textureSize;

	blib::ResourceManager* resourceManager;

	BrowEdit* browEdit;
	std::map<std::string, std::string> textureFiles;

	json lights;
	json effects;

	blib::wm::widgets::TreeView::TreeNode* newModelsNode;
	blib::wm::widgets::TreeView::TreeNode* newSoundsNode;
	blib::wm::widgets::TreeView::TreeNode* newEffectsNode;
	blib::wm::widgets::TreeView::TreeNode* newLightsNode;



	blib::wm::widgets::TreeView::TreeNode* modelsNode;
	blib::wm::widgets::TreeView::TreeNode* soundsNode;
	blib::wm::widgets::TreeView::TreeNode* effectsNode;
	blib::wm::widgets::TreeView::TreeNode* lightsNode;
public:
	ObjectWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);
	~ObjectWindow();

	void updateObjects(Map* map);
	void setModelDirectory(const std::string &dir);
	void setLightDirectory(std::string dir);
	void setEffectDirectory(std::string dir);

	virtual void arrangeComponents(int oldWidth, int oldHeight);
};