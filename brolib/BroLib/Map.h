#pragma once

#include <string>
class Gnd;
class Rsw;

class Map
{
	Gnd* gnd;
	Rsw* rsw;

	std::string fileName;
public:
	Map(const std::string &fileName);

	inline Gnd* getGnd() const
	{
		return gnd;
	}
	inline Rsw* getRsw() const
	{
		return rsw;
	}
	void save(const std::string &filename);

};