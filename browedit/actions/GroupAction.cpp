#include "GroupAction.h"


void GroupAction::perform(Map* map, MapRenderer& renderer)
{
	for (Action* a : actions)
		a->perform(map, renderer);
}

void GroupAction::undo(Map* map, MapRenderer& renderer)
{
	for (Action* a : actions)
		a->undo(map, renderer);
}

void GroupAction::add(Action* action)
{
	actions.push_back(action);
}

