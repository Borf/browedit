#include "Map.h"
#include "Gnd.h"

#include <blib/BackgroundTask.h>
#include <blib/util/Log.h>
using blib::util::Log;

Map::Map( const std::string &fileName )
{
	Log::out<<"Map: loading map "<<fileName<<Log::newline;
	blib::BackgroundTask* gndTask = new blib::BackgroundTask(NULL, [this, fileName]() { gnd = new Gnd(fileName); });
	blib::BackgroundTask* rswTask = new blib::BackgroundTask(NULL, [this, fileName]() {  });
	blib::BackgroundTask* gatTask = new blib::BackgroundTask(NULL, [this, fileName]() {  });


	gndTask->waitForTermination();
	rswTask->waitForTermination();
	gatTask->waitForTermination();

	Log::out<<"Map: Done loading map"<<Log::newline;

}

