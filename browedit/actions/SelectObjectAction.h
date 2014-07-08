#pragma once

#include "../action.h"
#include <vector>

class Rsw;

class SelectObjectAction : public Action
{
	std::vector<int> indices;
	std::vector<int> oldIndices;
public:
	SelectObjectAction::SelectObjectAction(Rsw* rsw);
	void finish(Rsw* rsw);

	bool hasDifference();

	virtual void perform(Map* map, MapRenderer& mapRenderer);
	virtual void undo(Map* map, MapRenderer& mapRenderer);
};