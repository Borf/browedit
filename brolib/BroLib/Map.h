#pragma once

#include <string>
class Gnd;

class Map
{
	Gnd* gnd;
public:
	Map(const std::string &fileName);
};