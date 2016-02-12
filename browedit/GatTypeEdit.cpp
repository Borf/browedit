#include "BrowEdit.h"
#include <BroLib/Map.h>
#include <BroLib/Gat.h>
#include <blib/wm/WM.h>


void BrowEdit::gatTypeEditUpdate()
{
	if (mouseState.leftButton && !wm->inWindow(mouseState.position))
	{
		int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 5);
		int cursorY = map->getGat()->height + 1 - (int)glm::floor(mapRenderer.mouse3d.z / 5);
		int mapHeight = map->getGat()->height;

		map->getGat()->tiles[cursorX][cursorY]->type = activeGatTile;
		mapRenderer.gatDirty = true;
	}
	for (char i = '0'; i <= '9'; i++)
	{
		if (keyState.isPressed((blib::Key)i))
			activeGatTile = i - '0';
	}
}