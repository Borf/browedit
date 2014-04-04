#pragma once

#include <string>
class Gnd;
class Rsw;

class Map
{
	Gnd* gnd;
	Rsw* rsw;
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


};