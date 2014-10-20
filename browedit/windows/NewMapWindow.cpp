#include "NewMapWindow.h"
#include "../BrowEdit.h"
#include <BroLib/Map.h>

#include <blib/wm/WM.h>
#include <blib/wm/widgets/button.h>
#include <blib/wm/widgets/textbox.h>

#include "TextureWindow.h"
#include "ObjectWindow.h"
#include "MessageWindow.h"


NewMapWindow::NewMapWindow(blib::ResourceManager* resourceManager, BrowEdit* browEdit) : Window("New Map", "NewMapWindow.json", resourceManager)
{
	blib::wm::WM::getInstance()->center(this);

	getComponent<blib::wm::widgets::Button>("btnCancel")->addClickHandler([this](int, int, int){
		close();
		return true;
	});

	getComponent<blib::wm::widgets::Button>("btnOk")->addClickHandler([this, resourceManager, browEdit](int, int, int){

		std::string name = getComponent<blib::wm::widgets::Textbox>("name")->text;
		int width = atoi(getComponent<blib::wm::widgets::Textbox>("width")->text.c_str());
		int height = atoi(getComponent<blib::wm::widgets::Textbox>("height")->text.c_str());

		if (width <= 0 || height <= 0)
		{
			new MessageWindow(resourceManager, "Invalid size...", "error");
			return true;
		}
		close();



		browEdit->map = new Map(name, height, width);

		Gnd::Texture* tex = new Gnd::Texture();
		tex->file = "black.bmp";
		tex->name = "black.bmp";
		browEdit->map->getGnd()->textures.push_back(tex);


		Gnd::Lightmap* lightmap = new Gnd::Lightmap();
		for (int i = 0; i < 256; i++)
			lightmap->data[i] = i >= 64 ? 0 : 255;

		browEdit->map->getGnd()->lightmaps.push_back(lightmap);

		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				Gnd::Tile* tile = new Gnd::Tile();
				tile->color = glm::vec3(1, 1, 1);
				tile->lightmapIndex = 0;
				tile->textureIndex = 0;
				tile->v1 = glm::vec2(0, 0);
				tile->v2 = glm::vec2(1, 0);
				tile->v3 = glm::vec2(0, 1);
				tile->v4 = glm::vec2(1, 1);
				browEdit->map->getGnd()->tiles.push_back(tile);
			}
		}


		for (int x = 0; x < browEdit->map->getGnd()->width; x++)
		{
			for (int y = 0; y < browEdit->map->getGnd()->height; y++)
			{
				int index = (x % 4) + 4 * (y % 4);
				browEdit->map->getGnd()->cubes[x][y]->tileUp = index;
			}
		}


		browEdit->mapRenderer.setMap(browEdit->map);

		browEdit->textureWindow->updateTextures(browEdit->map);
		browEdit->objectWindow->updateObjects(browEdit->map);


		return true;
	});

}

