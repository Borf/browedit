#include "BrowEdit.h"

#include <blib/wm/WM.h>
#include <blib/wm/Menu.h>
#include <blib/wm/ToggleMenuItem.h>
#include <blib/math/Polygon.h>
#include <blib/Math.h>

#include <BroLib/Map.h>
#include <BroLib/Gat.h>

//TODO: move these to their own files when visual studio decides to start working again
void BrowEdit::gatEditUpdate()
{
	if (!wm->inWindow(mouseState.position))
	{
		if (mouseState.leftButton && !lastMouseState.leftButton)
		{//down
			mouse3dstart = mapRenderer.mouse3d;
		}
		else if (!mouseState.leftButton && lastMouseState.leftButton)
		{//up
			if (!selectLasso.empty())
			{
				blib::math::Polygon polygon;
				for (size_t i = 0; i < selectLasso.size(); i++)
					polygon.push_back(glm::vec2(selectLasso[i]));
				for (int x = 0; x < map->getGat()->width; x++)
				{
					for (int y = 0; y < map->getGat()->height; y++)
					{
						map->getGat()->tiles[x][y]->selected = polygon.contains(glm::vec2(x, y));
					}
				}
				for (size_t i = 0; i < selectLasso.size(); i++)
					map->getGat()->tiles[selectLasso[i].x][selectLasso[i].y]->selected = true;
				selectLasso.clear();
			}
		}
		else if (mouseState.leftButton && lastMouseState.leftButton)
		{//drag
			if (keyState.isPressed(blib::Key::CONTROL))
			{
				int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 5);
				int cursorY = map->getGat()->height - (int)glm::floor(mapRenderer.mouse3d.z / 5) + 1;
				if (cursorX >= 0 && cursorX < map->getGat()->width && cursorY >= 0 && cursorY < map->getGat()->height)
				{
					if (selectLasso.empty())
					{
						selectLasso.push_back(glm::ivec2(cursorX, cursorY));
					}
					else
					{
						if (selectLasso[selectLasso.size() - 1] != glm::ivec2(cursorX, cursorY))
						{
							selectLasso.push_back(glm::ivec2(cursorX, cursorY));
						}
					}
				}
			}
			else
			{
				glm::vec2 tl = glm::vec2(glm::floor(glm::min(mouse3dstart.x, mapRenderer.mouse3d.x) / 5.0f) - 1, glm::floor(glm::min(mouse3dstart.z, mapRenderer.mouse3d.z) / 5.0f) - 2);
				glm::vec2 br = glm::vec2(glm::ceil(glm::max(mouse3dstart.x, mapRenderer.mouse3d.x) / 5.0f), glm::ceil(glm::max(mouse3dstart.z, mapRenderer.mouse3d.z) / 5.0f) - 1);
				blib::math::Rectangle rect(tl, br);

				for (int x = 0; x < map->getGat()->width; x++)
				{
					for (int y = 0; y < map->getGat()->height; y++)
					{
						map->getGat()->tiles[x][y]->selected = rect.contains(glm::vec2(x, map->getGat()->height - y));
					}
				}
			}
		}

		if (!mouseState.rightButton && lastMouseState.rightButton)
		{
			bool moved = false;
			for (int x = 0; x < map->getGat()->width; x++)
			{
				for (int y = 0; y < map->getGat()->height; y++)
				{
					if (!map->getGat()->tiles[x][y]->selected)
						continue;
					moved = true;
					for (int xx = -1; xx <= 1; xx++)
						for (int yy = -1; yy <= 1; yy++)
							mapRenderer.setTileDirty(x + xx, y + yy);
				}
			}
			if (moved)
				mapRenderer.gndGridDirty = true;
		}
		else if (mouseState.rightButton && lastMouseState.rightButton)
		{
			bool around = dynamic_cast<blib::wm::ToggleMenuItem*>(rootMenu->getItem("heighttools/connect"))->getValue();

			float diff = (float)(mouseState.position.y - lastMouseState.position.y);
			bool moved = false;
			for (int x = 0; x < map->getGat()->width; x++)
			{
				for (int y = 0; y < map->getGat()->height; y++)
				{
					if (!map->getGat()->tiles[x][y]->selected)
					{
						if (around)
						{
							bool changedCorners[4] = { false, false, false, false };
							for (int xx = -1; xx <= 1; xx++)
							{
								for (int yy = -1; yy <= 1; yy++)
								{
									if (x + xx < 0 || x + xx >= map->getGat()->width ||
										y + yy < 0 || y + yy >= map->getGat()->height)
										continue;
									if (!map->getGat()->tiles[x + xx][y + yy]->selected)
										continue;

									if (((xx == 1 && yy != 1) || (xx == 0 && yy == -1)) && !changedCorners[1])
									{
										map->getGat()->tiles[x][y]->heights[1] += diff;
										changedCorners[1] = true;
									}
									if (((xx == 1 && yy != -1) || (xx == 0 && yy == 1)) && !changedCorners[3])
									{
										map->getGat()->tiles[x][y]->heights[3] += diff;
										changedCorners[3] = true;
									}

									if (((xx == -1 && yy != 1) || (xx == 0 && yy == -1)) && !changedCorners[0])
									{
										map->getGat()->tiles[x][y]->heights[0] += diff;
										changedCorners[0] = true;
									}

									if (((xx == -1 && yy != -1) || (xx == 0 && yy == 1)) && !changedCorners[2])
									{
										map->getGat()->tiles[x][y]->heights[2] += diff;
										changedCorners[2] = true;
									}
								}
							}
						}
						continue;
					}
					moved = true;
					for (int i = 0; i < 4; i++)
						map->getGat()->tiles[x][y]->heights[i] += diff;
				}
			}
		}
	}
	if (true /* no keyboard focus in WM */)
	{
		if ((keyState.isPressed(blib::Key::C) && !lastKeyState.isPressed(blib::Key::C)) ||
			(keyState.isPressed(blib::Key::R) && !lastKeyState.isPressed(blib::Key::R)))
		{
			for (int x = 0; x < map->getGat()->width; x++)
			{
				for (int y = 0; y < map->getGat()->height; y++)
				{
					Gat::Tile* c = map->getGat()->tiles[x][y];
					if (c->selected)
					{
						float avgs[4] = { 0, 0, 0, 0 };
						for (int i = 0; i < 4; i++)
						{
							int count = 0;
							for (int ii = 0; ii < 4; ii++)
							{
								int xx = x + (-(ii % 2)) + (i % 2);
								int yy = y + (-(ii / 2)) + (i / 2);
								if (map->inMap(xx, yy))
								{
									avgs[i] += map->getGat()->tiles[xx][yy]->heights[ii];
									count++;
								}
							}
							avgs[i] /= count;
							if (keyState.isPressed(blib::Key::R))
								avgs[i] += blib::math::randomFloat(-2, 2);

						}

						for (int i = 0; i < 4; i++)
						{
							for (int ii = 0; ii < 4; ii++)
							{
								int xx = x + (-(ii % 2)) + (i % 2);
								int yy = y + (-(ii / 2)) + (i / 2);
								if (map->inMap(xx, yy))
								{
									map->getGat()->tiles[xx][yy]->heights[ii] = avgs[i];
									mapRenderer.setTileDirty(xx, yy);
								}
							}
						}
					}
				}
			}
			mapRenderer.gndGridDirty = true;
		} //end connecting/randomize
		if (keyState.isPressed(blib::Key::S) && !lastKeyState.isPressed(blib::Key::S))
		{
			std::map<glm::ivec2, float, bool(*)(const glm::ivec2&, const glm::ivec2&)> newHeights([](const glm::ivec2& a, const glm::ivec2& b){ return a.x + 5000 * a.y < b.x + 5000 * b.y; });
			for (int x = 0; x < map->getGat()->width; x++)
			{
				for (int y = 0; y < map->getGat()->height; y++)
				{
					Gat::Tile* c = map->getGat()->tiles[x][y];
					float avg = 0;
					int count = 0;
					for (int ii = 0; ii < 4; ii++)
					{
						int xx = x + (-(ii % 2));
						int yy = y + (-(ii / 2));
						if (map->inMap(xx, yy))
						{
							avg += map->getGat()->tiles[xx][yy]->heights[ii];
							count++;
						}
					}
					newHeights[glm::ivec2(x, y)] = avg / count;
				}
			}
			for (int x = 0; x < map->getGat()->width; x++)
			{
				for (int y = 0; y < map->getGat()->height; y++)
				{
					Gat::Tile* c = map->getGat()->tiles[x][y];
					if (!c->selected)
						continue;
					for (int ii = 0; ii < 4; ii++)
					{
						int xx = x + (-(ii % 2));
						int yy = y + (-(ii / 2));
						if (map->inMap(xx, yy))
						{
							int count = 0;
							float total = 0;
							for (int xxx = -1; xxx <= 1; xxx++)
							{
								for (int yyy = -1; yyy <= 1; yyy++)
								{
									if (map->inMap(x + xxx, y + yyy))
									{
										total += newHeights[glm::ivec2(x + xxx, y + yyy)];
										count++;
									}
								}
							}
							map->getGat()->tiles[xx][yy]->heights[ii] = total / count;
							mapRenderer.setTileDirty(xx, yy);
						}
					}
				}
			}
			mapRenderer.gndGridDirty = true;
		}
		if (keyState.isPressed(blib::Key::F) && !lastKeyState.isPressed(blib::Key::F))
		{
			float avg = 0;
			int count = 0;
			for (int x = 0; x < map->getGat()->width; x++)
			{
				for (int y = 0; y < map->getGat()->height; y++)
				{
					Gat::Tile* c = map->getGat()->tiles[x][y];
					if (!c->selected)
						continue;
					for (int i = 0; i < 4; i++)
						avg += c->heights[i];
					count += 4;
				}
			}
			avg /= count;
			for (int x = 0; x < map->getGat()->width; x++)
			{
				for (int y = 0; y < map->getGat()->height; y++)
				{
					Gat::Tile* c = map->getGat()->tiles[x][y];
					if (!c->selected)
						continue;
					for (int i = 0; i < 4; i++)
						c->heights[i] = avg;
					mapRenderer.setTileDirty(x, y);
				}
			}
			mapRenderer.gndGridDirty = true;
		}

	}
}