#include "BrowEdit.h"

#include <blib/wm/WM.h>

#include <BroLib/Map.h>
#include <BroLib/MapRenderer.h>

#include "windows/TextureWindow.h"

#include "actions/TextureEditAction.h"


void BrowEdit::textureEditUpdate()
{
	if (keyState.isPressed(blib::Key::R) && !lastKeyState.isPressed(blib::Key::R))
		textureRot = (textureRot + 1) % 4;
	if (keyState.isPressed(blib::Key::H) && !lastKeyState.isPressed(blib::Key::H))
		textureFlipH = !textureFlipH;
	if (keyState.isPressed(blib::Key::V) && !lastKeyState.isPressed(blib::Key::V))
		textureFlipV = !textureFlipV;
	if ((keyState.isPressed(blib::Key::PLUS) && !lastKeyState.isPressed(blib::Key::PLUS)) || 
		(keyState.isPressed(blib::Key::EQUALS) && !lastKeyState.isPressed(blib::Key::EQUALS)) ||
		(keyState.isPressed(blib::Key::BRACKETRIGHT) && !lastKeyState.isPressed(blib::Key::BRACKETRIGHT))
		)
		textureTargetSize += glm::ivec2(1, 1);
	if ((keyState.isPressed(blib::Key::MINUS) && !lastKeyState.isPressed(blib::Key::MINUS)) || 
		(keyState.isPressed(blib::Key::UNDERSCORE) && !lastKeyState.isPressed(blib::Key::UNDERSCORE)) ||
		(keyState.isPressed(blib::Key::BRACKETLEFT) && !lastKeyState.isPressed(blib::Key::BRACKETLEFT))

		)
		textureTargetSize = glm::max(glm::ivec2(1, 1), textureTargetSize + glm::ivec2(-1, -1));
	if (keyState.isPressed(blib::Key::LEFT) && !lastKeyState.isPressed(blib::Key::LEFT))
		textureTargetSize = glm::max(glm::ivec2(1, 1), textureTargetSize + glm::ivec2(-1, 0));
	if (keyState.isPressed(blib::Key::RIGHT) && !lastKeyState.isPressed(blib::Key::RIGHT))
		textureTargetSize += glm::ivec2(1, 0);
	if (keyState.isPressed(blib::Key::UP) && !lastKeyState.isPressed(blib::Key::UP))
		textureTargetSize += glm::ivec2(0, 1);
	if (keyState.isPressed(blib::Key::DOWN) && !lastKeyState.isPressed(blib::Key::DOWN))
		textureTargetSize = glm::max(glm::ivec2(1, 1), textureTargetSize + glm::ivec2(0, -1));

	int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
	int cursorY = map->getGnd()->height + 1 - (int)glm::floor(mapRenderer.mouse3d.z / 10);
	int mapHeight = map->getGnd()->height;


	if (mouseState.leftButton && !lastMouseState.leftButton && !wm->inWindow(mouseState.position) && textureWindow->selectedImage != -1)
	{
		int cursorWidth = textureTargetSize.x;
		int cursorHeight = textureTargetSize.y;

		glm::ivec2 cursorTopLeft = glm::ivec2(cursorX, cursorY) - textureTargetSize / 2;

		glm::vec2 texStart = glm::vec2(textureWindow->tx1.x, textureWindow->tx2.y);
		glm::vec2 texInc = (textureWindow->tx2 - textureWindow->tx1) / glm::vec2(cursorWidth, -cursorHeight);
		glm::vec2 texCenter = (textureWindow->tx1 + textureWindow->tx2) / 2.0f;

		glm::mat4 rot;
		rot = glm::translate(rot, glm::vec3(texCenter, 0));
		rot = glm::rotate(rot, glm::radians(90.0f * textureRot), glm::vec3(0, 0, 1));
		rot = glm::scale(rot, glm::vec3(textureFlipH ? -1 : 1, textureFlipV ? -1 : 1, 1));
		rot = glm::translate(rot, glm::vec3(-texCenter, 0));


		TextureAction* action = new TextureAction();

		for (int x = 0; x < cursorWidth; x++)
		{
			for (int y = 0; y < cursorHeight; y++)
			{
				int xx = x + cursorTopLeft.x;
				int yy = y + cursorTopLeft.y;
				if (xx < 0 || yy < 0 || xx >= map->getGnd()->width || yy >= map->getGnd()->height)
					continue;

				glm::vec2 t1 = texStart + glm::vec2(x, y) * texInc;
				glm::vec2 t2 = t1 + texInc;

				Gnd::Cube* cube = map->getGnd()->cubes[xx][yy];
				Gnd::Tile tile;
				if (cube->tileUp != -1)
					tile = Gnd::Tile(*map->getGnd()->tiles[cube->tileUp]);
				else
				{
					tile = Gnd::Tile();
					tile.lightmapIndex = 0;
				}
				tile.textureIndex = textureWindow->selectedImage;
				tile.v1 = glm::vec2(rot * glm::vec4(t1.x, t1.y, 0, 1));
				tile.v2 = glm::vec2(rot * glm::vec4(t2.x, t1.y, 0, 1));
				tile.v3 = glm::vec2(rot * glm::vec4(t1.x, t2.y, 0, 1));
				tile.v4 = glm::vec2(rot * glm::vec4(t2.x, t2.y, 0, 1));
				action->addTile(xx, yy, tile);

			}
		}

		perform(action);

	}
}
