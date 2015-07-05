#include "BrowEdit.h"

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include "windows/TextureWindow.h"


//tilefront = between x and x+1

void BrowEdit::wallEditUpdate()
{
	if (keyState.isPressed(blib::Key::LEFT) && !lastKeyState.isPressed(blib::Key::LEFT))
		newTextureSize = glm::max(newTextureSize-1, 1);
	if (keyState.isPressed(blib::Key::RIGHT) && !lastKeyState.isPressed(blib::Key::RIGHT))
		newTextureSize++;


	if (mouseState.leftButton && !lastMouseState.leftButton)
	{
		mouse3dstart = mapRenderer.mouse3d;


	}
	else if (!mouseState.leftButton && lastMouseState.leftButton)
	{//up
		if (keyState.isPressed(blib::Key::SHIFT))
		{
			int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
			int cursorY = map->getGnd()->height - (int)glm::round(mapRenderer.mouse3d.z / 10);

			float diffx = cursorX - (mapRenderer.mouse3d.x / 10.0f);
			float diffy = cursorY - (map->getGnd()->height - mapRenderer.mouse3d.z / 10.0f);

			if (diffx < -0.5f)
				diffx++;
			if (diffy > 0.5f)
				diffy--;

			bool horizontal = fabs(diffx) > fabs(diffy);

			//todo: calculate extrax and extray  using a ray-plane collision, and determine the direction the normal is pointing at
			bool extrax = !horizontal && diffx > 0;
			bool extray = horizontal && diffy > 0;



			if (fabs(diffx) < 0.01 || fabs(diffy) < 0.01)
			{
				if (fabs(diffx) < fabs(diffy))
				{
					cursorX = (int)glm::round(mapRenderer.mouse3d.x / 10) - 1;
					cursorY = map->getGnd()->height - (int)glm::floor(mapRenderer.mouse3d.z / 10);
				}

				std::vector<blib::VertexP3T2> verts;
				Gnd* gnd = map->getGnd();
				int x = cursorX;
				int y = cursorY;
				int xx = 0;
				int yy = 0;

				for (int i = 0; i < newTextureSize; i++)
				{
					if (!map->inMap(x + xx, y + yy))
						continue;
					Gnd::Cube* cube = gnd->cubes[x + xx][y + yy];


					float tx1, tx2;
					float ty1, ty2;

					if (extrax || extray)
					{
						tx1 = textureWindow->tx1.x + i * ((textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize);
						tx2 = tx1 + (textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize;
						ty1 = textureWindow->tx2.y;
						ty2 = textureWindow->tx1.y;
					}
					else
					{
						tx2 = textureWindow->tx1.x + i * ((textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize);
						tx1 = tx2 + (textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize;
						ty1 = textureWindow->tx1.y;
						ty2 = textureWindow->tx2.y;
					}

					if (cube->tileFront != -1 && x < gnd->width - 1 && !horizontal)
					{
						Gnd::Tile* tile = new Gnd::Tile();
						cube->tileFront = gnd->tiles.size();
						gnd->tiles.push_back(tile);

						tile->textureIndex = textureWindow->selectedImage;
						tile->lightmapIndex = 0;
						tile->color = glm::ivec4(255, 255, 255,255);
						tile->v1 = glm::vec2(tx1, ty1);
						tile->v2 = glm::vec2(tx2, ty1);
						tile->v3 = glm::vec2(tx1, ty2);
						tile->v4 = glm::vec2(tx2, ty2);
					}
					if (cube->tileSide != -1 && y < gnd->height - 1 && horizontal)
					{
						Gnd::Tile* tile = new Gnd::Tile();
						cube->tileSide = gnd->tiles.size();
						gnd->tiles.push_back(tile);

						tile->textureIndex = textureWindow->selectedImage;
						tile->lightmapIndex = 0;
						tile->color = glm::ivec4(255, 255, 255, 255);
						tile->v1 = glm::vec2(tx1, ty1);
						tile->v2 = glm::vec2(tx2, ty1);
						tile->v3 = glm::vec2(tx1, ty2);
						tile->v4 = glm::vec2(tx2, ty2);
					}

					mapRenderer.setTileDirty(x + xx, y + yy);
					if (horizontal)
						xx += extray ? 1 : -1;
					else
						yy += extrax ? -1 : 1;
				}
			}

		}
		else //no shiftkey pressed...place or remove walls
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
						if (!map->inMap(x, y))
							continue;
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
							t->color = glm::ivec4(255, 255, 255, 255);

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