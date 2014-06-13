#pragma once

#include "../action.h"
#include <BroLib/Gnd.h>
#include <list>

class TextureAction : public Action
{
	class Data
	{
	public:
		int x;
		int y;
		Gnd::Tile tile;

		Data(int x, int y, const Gnd::Tile& tile)
		{
			this->x = x;
			this->y = y;
			this->tile = tile;
		}
	};

	class UndoData
	{
	public:
		int x;
		int y;
		int oldTile;

		UndoData(int x, int y, int oldTile)
		{
			this->x = x;
			this->y = y;
			this->oldTile = oldTile;
		}
	};

	std::list<Data> data;
	std::list<UndoData> undodata;
public:
	void addTile(int x, int y, const Gnd::Tile &tile);

	virtual void perform(Map* map, MapRenderer& mapRenderer);
	virtual void undo(Map* map, MapRenderer& mapRenderer);
};