#pragma once


#include <blib/wm/Window.h>
#include <blib/wm/widgets/Image.h>

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <map>

class BrowEdit;
class Map;



class ObjectWindow : public blib::wm::Window
{
	int smallWidth;
	int largeWidth;

	blib::ResourceManager* resourceManager;

	BrowEdit* browEdit;
	std::map<std::string, std::string> textureFiles;

public:
	ObjectWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit);
	~ObjectWindow();

	void updateObjects(Map* map);

	virtual void arrangeComponents(int oldWidth, int oldHeight);
};