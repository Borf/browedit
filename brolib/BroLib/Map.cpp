#include "Map.h"
#include "Gnd.h"
#include "Rsw.h"
#include "Gat.h"

#include <blib/BackgroundTask.h>
#include <blib/util/Log.h>
using blib::util::Log;

Map::Map( const std::string &fileName )
{
	this->fileName = fileName;
	Log::out<<"Map: loading map "<<fileName<<Log::newline;

	//TODO: for some reason it doesn't like to parallalize these tasks...investigate later
#if 0
	blib::BackgroundTask<int>* gndTask = new blib::BackgroundTask<int>(NULL, [this, fileName]() { gnd = new Gnd(fileName); return 0; });
	blib::BackgroundTask<int>* rswTask = new blib::BackgroundTask<int>(NULL, [this, fileName]() { rsw = new Rsw(fileName); return 0; });
	blib::BackgroundTask<int>* gatTask = new blib::BackgroundTask<int>(NULL, [this, fileName]() { gat = new Gat(fileName); return 0; });
	gndTask->waitForTermination();
	rswTask->waitForTermination();
	gatTask->waitForTermination();
#else
	gnd = new Gnd(fileName);
	rsw = new Rsw(fileName);
	gat = new Gat(fileName);
#endif


	Log::out<<"Map: Done loading map"<<Log::newline;

}

void Map::save(const std::string &filename)
{
	if (filename != "")
		this->fileName = filename;

	gnd->save(this->fileName);
	rsw->recalculateQuadTree(gnd);
	rsw->save(this->fileName);
	gat->save(this->fileName);
}

template class blib::BackgroundTask<int>;

