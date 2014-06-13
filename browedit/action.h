#pragma once


class Map;

class Action
{
public:
	virtual void perform(Map* map) = 0;
	virtual void undo(Map* map) = 0;
};