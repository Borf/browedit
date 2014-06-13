#include "TextureEditAction.h"
#include <BroLib/Map.h>
#include <BroLib/MapRenderer.h>



void TextureAction::addTile(int x, int y, const Gnd::Tile &tile)
{
	data.push_back(Data(x,y,tile));
}

void TextureAction::perform(Map* map, MapRenderer& mapRenderer)
{
	for (std::list<Data>::iterator it = data.begin(); it != data.end(); it++)
	{
		Gnd::Cube* cube = map->getGnd()->cubes[it->x][it->y];
		undodata.push_back(UndoData(it->x, it->y, cube->tileUp));
		cube->tileUp = map->getGnd()->tiles.size();
		map->getGnd()->tiles.push_back(new Gnd::Tile(it->tile));
		mapRenderer.setTileDirty(it->x, it->y);
	}

}

void TextureAction::undo(Map* map, MapRenderer& mapRenderer)
{
	for (std::list<UndoData>::iterator it = undodata.begin(); it != undodata.end(); it++)
	{
		map->getGnd()->cubes[it->x][it->y]->tileUp = it->oldTile;
		mapRenderer.setTileDirty(it->x, it->y);
	}
	undodata.clear();
}

