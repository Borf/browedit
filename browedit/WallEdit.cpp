#include "BrowEdit.h"

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>



//tilefront = between x and x+1

void BrowEdit::wallEditUpdate()
{
	if (mouseState.leftButton && !lastMouseState.leftButton)
	{
		mouse3dstart = mapRenderer.mouse3d;


	}
	else if (mouseState.rightButton && !lastMouseState.rightButton)
	{
		mouse3dstart = mapRenderer.mouse3d;


	}

	else if (!mouseState.rightButton && lastMouseState.rightButton)
	{//up
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
					if (cursorX == lastCursorX)
						map->getGnd()->cubes[x][y]->tileFront = keyState.isPressed(blib::Key::CONTROL) ? -1 : 0;
					else
						map->getGnd()->cubes[x][y]->tileSide = keyState.isPressed(blib::Key::CONTROL) ? -1 : 0;
					mapRenderer.setTileDirty(x, y);
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