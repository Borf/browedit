#include "BrowEdit.h"

#include <blib/wm/WM.h>

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include "windows/ColorWindow.h"

void BrowEdit::colorEditUpdate()
{
	if ((mouseState.leftButton || lastMouseState.rightButton) && !wm->inWindow(mouseState.position))
	{
		int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
		int cursorY = map->getGnd()->height - (int)glm::floor(mapRenderer.mouse3d.z / 10);

		static int lastCursorX = -1;
		static int lastCursorY = -1;

		if (lastCursorX != cursorX || lastCursorY != cursorY || mouseState.position != lastMouseState.position || (mouseState.leftButton && !lastMouseState.leftButton) || (mouseState.rightButton && !lastMouseState.rightButton))
		{
			if (map->inMap(cursorX, cursorY))
			{
				int tileUp = map->getGnd()->cubes[cursorX][cursorY]->tileUp;
				if (tileUp > -1)
				{
					Gnd::Tile* tile = map->getGnd()->tiles[tileUp];
					tile->color = glm::vec4(colorWindow->color * 255.0f, 255.0f);
					mapRenderer.setColorDirty();
				}
			}
		}
		lastCursorX = cursorX;
		lastCursorY = cursorY;
	}
}
