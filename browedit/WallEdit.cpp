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

	else if (!mouseState.leftButton && lastMouseState.leftButton)
	{//up
		int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
		int cursorY = map->getGnd()->height - (int)glm::floor(mapRenderer.mouse3d.z / 10);
		if (map->inMap(cursorX, cursorY))
		{
			map->getGnd()->cubes[cursorX][cursorY]->tileFront = -1;
			mapRenderer.setTileDirty(cursorX, cursorY);
		}

	}

}