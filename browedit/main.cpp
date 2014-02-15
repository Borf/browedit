#include <blib/util/FileSystem.h>
#include <blib/Util.h>
#include "BrowEdit.h"
#include <BroLib/GrfFileSystemHandler.h>

int main()
{
	blib::util::fixConsole();
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(""));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(".."));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler("../blib"));

	blib::util::FileSystem::registerHandler(new GrfFileSystemHandler("c:/games/CookieRO/data.grf"));

	BrowEdit* app = new BrowEdit();
	app->start();
	delete app;
	return 0;
}