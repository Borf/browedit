#ifdef WIN32
#include <Windows.h>
#include <blib/platform/win32/Registry.h>
#include <iostream>
#endif

#include <blib/util/FileSystem.h>
#include <blib/Util.h>
#include "BrowEdit.h"
#include <BroLib/GrfFileSystemHandler.h>
#include <blib/util/Log.h>
using blib::util::Log;



void mergeConfig(Json::Value &config, const Json::Value &newConfig);


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

	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(""));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(".."));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler("../blib"));

	Log::out<<"Loading configuration..."<<Log::newline;
	Json::Value config = blib::util::FileSystem::getJson("assets/configs/config.default.json");
#ifdef WIN32
	blib::platform::win32::RegistryKey key( HKEY_CURRENT_USER, "Software\\Browedit" );
	if( !key.exists() && !key.create() ){
		// Something went wrong
	}
	blib::platform::win32::RegistryValue value( key, "config" );
	std::string configFileName = value.readString( "" );
	while( !blib::util::FileSystem::exists( "assets/configs/" + configFileName ) )
	{
		// we ask him for his choice
		Log::err<< "Unable to find configuration file, please type the configuration filename" << Log::newline;
		std::getline( std::cin, configFileName );
	}
	value.writeString( configFileName );

	mergeConfig( config, blib::util::FileSystem::getJson( "assets/configs/" + configFileName ) );
#endif

	if (config["moveconsole"].asBool())
		blib::util::fixConsole();





	BrowEdit* app = new BrowEdit(config);
	app->start();
	delete app;
	return 0;
}



	void mergeConfig(Json::Value &config, const Json::Value &newConfig)
	{
		Json::Value::Members values = newConfig.getMemberNames();
		for(size_t i = 0; i < values.size(); i++)
			if(config.isMember(values[i]))
				if(config[values[i]].isObject())
					mergeConfig(config[values[i]], newConfig[values[i]]);
				else
					config[values[i]] = newConfig[values[i]];
			else
				config[values[i]] = newConfig[values[i]];
	}