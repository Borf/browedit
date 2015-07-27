#include "BrowEdit.h"

#include <blib/wm/ToggleMenuItem.h>
#include <blib/wm/Menu.h>
#include <BroLib/Map.h>
#include <BroLib/MapRenderer.h>
#include <BroLib/Gnd.h>


void BrowEdit::detailHeightEditUpdate()
{
	if (mouseState.leftButton && !lastMouseState.leftButton) //down
	{
		int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
		int cursorY = map->getGnd()->height - (int)glm::floor(mapRenderer.mouse3d.z / 10);

		float cursorXOff = -(cursorX - (mapRenderer.mouse3d.x / 10));
		float cursorYOff = cursorY - (map->getGnd()->height - (mapRenderer.mouse3d.z / 10));

		detailHeightCursor = glm::ivec2(cursorX, cursorY);
		detailHeightOffset = glm::vec2(cursorXOff, cursorYOff);

	}
	else if (mouseState.leftButton && lastMouseState.leftButton)//drag
	{
		Gnd* gnd = map->getGnd();
		if (map->inMap(detailHeightCursor.x, detailHeightCursor.y))
		{

			auto changeHeight = [this, gnd](const glm::ivec2 &pos, const glm::vec2 &off, float diff){
				if (!map->inMap(pos.x, pos.y))
					return;
				Gnd::Cube* cube = gnd->cubes[pos.x][pos.y];
				int primIndex = 0;
				if (off.x < 0.5 && off.y > 0.5)
					primIndex = 0;
				else if (off.x > 0.5 && off.y > 0.5)
					primIndex = 1;
				else if (off.x < 0.5 && off.y < 0.5)
					primIndex = 2;
				else if (off.x > 0.5 && off.y < 0.5)
					primIndex = 3;

				cube->heights[primIndex] += diff;
			};

			float diff = (mouseState.position.y - lastMouseState.position.y) / 10.0f;

			changeHeight(detailHeightCursor, detailHeightOffset, diff);
			if (dynamic_cast<blib::wm::ToggleMenuItem*>(rootMenu->getItem("heighttools/connect"))->getValue())
			{
				if (detailHeightOffset.x < 0.5)
				{
					changeHeight(detailHeightCursor + glm::ivec2(-1, 0), glm::vec2(1, detailHeightOffset.y), diff);
					if (detailHeightOffset.y > 0.5)
						changeHeight(detailHeightCursor + glm::ivec2(-1, -1), glm::vec2(1, 0), diff);
					else
						changeHeight(detailHeightCursor + glm::ivec2(-1, 1), glm::vec2(1, 1), diff);
				}
				else
				{
					changeHeight(detailHeightCursor + glm::ivec2(1, 0), glm::vec2(0, detailHeightOffset.y), diff);
					if (detailHeightOffset.y > 0.5)
						changeHeight(detailHeightCursor + glm::ivec2(1, -1), glm::vec2(0, 0), diff);
					else
						changeHeight(detailHeightCursor + glm::ivec2(1, 1), glm::vec2(0, 1), diff);
				}

				if (detailHeightOffset.y > 0.5)
					changeHeight(detailHeightCursor + glm::ivec2(0, -1), glm::vec2(detailHeightOffset.x, 0), diff);
				else
					changeHeight(detailHeightCursor + glm::ivec2(0, 1), glm::vec2(detailHeightOffset.x, 1), diff);
			}

			mapRenderer.gndGridDirty = true;
		}
	}
	else if (!mouseState.leftButton && lastMouseState.leftButton) //up
	{
		for (int x = -1; x <= 1; x++)
			for (int y = -1; y <= 1; y++)
				mapRenderer.setTileDirty(detailHeightCursor.x+x, detailHeightCursor.y+y);
	}
}