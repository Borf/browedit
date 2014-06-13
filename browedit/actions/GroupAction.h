#pragma once

#include "../action.h"

class GroupAction : public Action
{
public:
	virtual void perform(Map* map);
	virtual void undo(Map* map);
};