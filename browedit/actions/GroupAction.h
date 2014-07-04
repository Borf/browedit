#pragma once

#include "../action.h"
#include <list>

class GroupAction : public Action
{
	std::list<Action*> actions;
public:
	virtual void perform(Map* map, MapRenderer& renderer);
	virtual void undo(Map* map, MapRenderer& renderer);

	void add(Action* action);
};