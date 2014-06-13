#pragma once


#include <blib/wm/Window.h>
#include <blib/wm/widgets/Image.h>

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <map>

class BrowEdit;
class Map;
class Rsm;

namespace blib { class FBO; };

class ModelWidget : public blib::wm::Widget
{
	Rsm* rsm;
	BrowEdit* browedit;

	blib::FBO* fbo;
	float rotation;
public:
	ModelWidget(Rsm* rsm, blib::ResourceManager* resourceManager, BrowEdit* browedit);
	~ModelWidget();
	virtual void draw(blib::SpriteBatch &spriteBatch, glm::mat4 matrix, blib::Renderer* renderer) const;
};


class ObjectWindow : public blib::wm::Window
{
	int smallWidth;
	int largeWidth;

	int textureSize;

	blib::ResourceManager* resourceManager;

	BrowEdit* browEdit;
	std::map<std::string, std::string> textureFiles;

public:
	ObjectWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);
	~ObjectWindow();

	void updateObjects(Map* map);
	void setDirectory(const std::string &dir);

	virtual void arrangeComponents(int oldWidth, int oldHeight);
};