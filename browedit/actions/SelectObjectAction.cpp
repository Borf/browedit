#include "SelectObjectAction.h"
#include <algorithm>
#include <BroLib/Map.h>
#include <BroLib/Rsw.h>


SelectObjectAction::SelectObjectAction(Rsw* rsw)
{
	for (size_t i = 0; i < rsw->objects.size(); i++)
		if (rsw->objects[i]->selected)
			oldIndices.push_back(i);
}

void SelectObjectAction::finish(Rsw* rsw)
{
	for (size_t i = 0; i < rsw->objects.size(); i++)
		if (rsw->objects[i]->selected)
			indices.push_back(i);
}


void SelectObjectAction::perform(Map* map, MapRenderer& mapRenderer)
{
	int ii = 0;
	for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
	{
		if (ii < indices.size())
		{
			map->getRsw()->objects[i]->selected = indices[ii] == i;
			if (indices[ii] == i)
				ii++;
		}
		else
			map->getRsw()->objects[i]->selected = false;
	}
}

void SelectObjectAction::undo(Map* map, MapRenderer& mapRenderer)
{
	int ii = 0;
	for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
	{
		if (ii < oldIndices.size())
		{
			map->getRsw()->objects[i]->selected = oldIndices[ii] == i;
			if (oldIndices[ii] == i)
				ii++;
		}
		else
			map->getRsw()->objects[i]->selected = false;
	}
}

bool SelectObjectAction::hasDifference()
{
	if (indices.size() != oldIndices.size())
		return true;
	return indices != oldIndices;
}

