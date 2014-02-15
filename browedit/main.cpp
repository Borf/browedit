#include <blib/util/FileSystem.h>
#include <blib/Util.h>
#include "BrowEdit.h"
#include <BroLib/GrfFileSystemHandler.h>
#include <blib/util/Log.h>
using blib::util::Log;

int main()
{
	blib::util::Thread::setMainThread();
	Log::out<<"/============================================================\\"<<Log::newline;
	Log::out<<"||     ____                    ______    _ _ _              ||"<<Log::newline;
	Log::out<<"||    |  _ \\                  |  ____|  | (_) |             ||"<<Log::newline;
	Log::out<<"||    | |_) |_ __ _____      _| |__   __| |_| |_            ||"<<Log::newline;
	Log::out<<"||    |  _ <| '__/ _ \\ \\ /\\ / /  __| / _` | | __|           ||"<<Log::newline;
	Log::out<<"||    | |_) | | | (_) \\ V  V /| |___| (_| | | |_            ||"<<Log::newline;
	Log::out<<"||    |____/|_|  \\___/ \\_/\\_/ |______\\__,_|_|\\__|	    ||"<<Log::newline;
	Log::out<<"||                                                          ||"<<Log::newline;
	Log::out<<"||                   Coded by:      Borf                    ||"<<Log::newline;
	Log::out<<"||                                                          ||"<<Log::newline;
	Log::out<<"||                     Libraries used:                      ||"<<Log::newline;
	Log::out<<"||            - OpenGL                                      ||"<<Log::newline;
	Log::out<<"||            - blib                                        ||"<<Log::newline;
	Log::out<<"||            - zlib                                        ||"<<Log::newline;
	Log::out<<"||            - grflib                                      ||"<<Log::newline;
	Log::out<<"||            - fastdeligate                                ||"<<Log::newline;
	Log::out<<"||            - glew                                        ||"<<Log::newline;
	Log::out<<"||            - glm                                         ||"<<Log::newline;
	Log::out<<"||            - jsoncpp                                     ||"<<Log::newline;
	Log::out<<"||            - stb_image                                   ||"<<Log::newline;
	Log::out<<"||            - (win)sockets                                ||"<<Log::newline;
	Log::out<<"||                                                          ||"<<Log::newline;
	Log::out<<"\\============================================================/"<<Log::newline;
	Log::out<<""<<Log::newline;
	Log::out<<""<<Log::newline;
	Log::out<<"       Not really a release, don't bug about any bugs..."<<Log::newline;
	Log::out<<Log::newline;

	blib::util::fixConsole();
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(""));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(".."));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler("../blib"));

	BrowEdit* app = new BrowEdit();
	app->start();
	delete app;
	return 0;
}