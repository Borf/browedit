#include "BrowEdit.h"

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include "windows/TextureWindow.h"


//tilefront = between x and x+1

void BrowEdit::wallEditUpdate()
{
	if (mouseState.leftButton && !lastMouseState.leftButton)
	{
		mouse3dstart = mapRenderer.mouse3d;


	}
	else if (!mouseState.leftButton && lastMouseState.leftButton)
	{//up
		if (keyState.isPressed(blib::Key::SHIFT))
		{
			
		}
		else //no shift
		{
			int lastCursorX = (int)glm::round(mouse3dstart.x / 10) - 1;
			int lastCursorY = map->getGnd()->height - (int)glm::round(mouse3dstart.z / 10);
			int cursorX = (int)glm::round(mapRenderer.mouse3d.x / 10) - 1;
			int cursorY = map->getGnd()->height - (int)glm::round(mapRenderer.mouse3d.z / 10);

			if (map->inMap(cursorX + 1, cursorY + 1))
			{

				std::vector<blib::VertexP3> verts;
				if (glm::abs(lastCursorX - cursorX) < glm::abs(lastCursorY - cursorY))
					cursorX = lastCursorX;
				else
					cursorY = lastCursorY;

				glm::vec3 lastPos;
				bool first = true;

				for (int x = glm::min(cursorX, lastCursorX); x <= glm::max(cursorX, lastCursorX); x++)
				{
					for (int y = glm::min(cursorY, lastCursorY); y <= glm::max(cursorY, lastCursorY); y++)
					{
						int newTile = -1;
						if (!keyState.isPressed(blib::Key::CONTROL))
						{
							Gnd::Tile* t = new Gnd::Tile();

							t->textureIndex = textureWindow->selectedImage >= 0 ? textureWindow->selectedImage : 0;
							t->v1 = glm::vec2(0, 0);
							t->v2 = glm::vec2(1, 0);
							t->v3 = glm::vec2(0, 1);
							t->v4 = glm::vec2(1, 1);
							t->lightmapIndex = 0;
							t->color = glm::vec4(1, 1, 1, 1);

							newTile = map->getGnd()->tiles.size();
							map->getGnd()->tiles.push_back(t);
						}
						if (cursorX == lastCursorX)
							map->getGnd()->cubes[x][y]->tileFront = newTile;
						else
							map->getGnd()->cubes[x][y]->tileSide = newTile;
						mapRenderer.setTileDirty(x, y);
					}
				}
			}
		}





		/*int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
		int cursorY = map->getGnd()->height - (int)glm::floor(mapRenderer.mouse3d.z / 10);
		if (map->inMap(cursorX, cursorY))
		{
			map->getGnd()->cubes[cursorX][cursorY]->tileFront = -1;
			mapRenderer.setTileDirty(cursorX, cursorY);
		}*/

	}

}