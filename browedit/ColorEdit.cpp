#include "BrowEdit.h"

#include <blib/wm/WM.h>

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include "windows/ColorWindow.h"

void BrowEdit::colorEditUpdate()
{
	if ((mouseState.leftButton || lastMouseState.rightButton) && !wm->inWindow(mouseState.position))
	{
		if (mouseState.position != lastMouseState.position || (mouseState.leftButton && !lastMouseState.leftButton) || (mouseState.rightButton && !lastMouseState.rightButton))
		{
			int x = mapRenderer.mouse3d.x / 10;
			int y = map->getGnd()->height - mapRenderer.mouse3d.z / 10;
			if (map->inMap(x, y))
			{
				int tileUp = map->getGnd()->cubes[x][y]->tileUp;
				if (tileUp > -1)
				{
					Gnd::Tile* tile = map->getGnd()->tiles[tileUp];
					tile->color = glm::vec4(colorWindow->color * 256.0f, 255.0f);
					mapRenderer.setColorDirty();
				}

			}
		}
	}
}
