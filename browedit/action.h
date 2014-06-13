#pragma once


class Map;
class MapRenderer;

class Action
{
public:
	virtual void perform(Map* map, MapRenderer& mapRenderer) = 0;
	virtual void undo(Map* map, MapRenderer& mapRenderer) = 0;
};