#pragma once

#include <vector>
#include <string>


class Gat
{
public:
	class Tile
	{
	public:
		float heights[4];
		int type;
		bool selected;
	};
	int version;
	int width;
	int height;
	std::vector<std::vector<Tile*>> tiles;




	Gat(const std::string &fileName);
	Gat(int width, int height);
	~Gat();

	void save(const std::string &fileName);

};