#pragma once

#include <vector>


class Gat
{
public:
	class Tile
	{
	public:
		float cells[4];
		int type;
	};
	int version;
	int width;
	int height;
	std::vector<std::vector<Tile*>> tiles;




	Gat(const std::string &fileName);
	void save(const std::string &fileName);

};