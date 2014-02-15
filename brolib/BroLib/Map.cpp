#include "Map.h"
#include "Gnd.h"


Map::Map( const std::string &fileName )
{
	gnd = new Gnd(fileName);
}

