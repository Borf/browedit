#include <blib/config.h>
#include <blib/util/FileSystem.h>
#include <blib/Util.h>
#include <blib/util/Log.h>

#ifdef BLIB_WIN
#include <Windows.h>
#include <blib/platform/win32/Registry.h>
#include <iostream>
#include "resource.h"
#endif

#include <v8.h>
#include "BrowEdit.h"
#ifndef _DEBUG
#include <BugTrap.h>
#endif

#include <BroLib/GrfFileSystemHandler.h>
using blib::util::Log;


#ifndef _DEBUG
#pragma comment(lib, "BugTrap.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "wininet.lib")
#endif

void mergeConfig(Json::Value &config, const Json::Value &newConfig);


#pragma comment(lib, "icui18n.lib")
#pragma comment(lib, "icuuc.lib")
#pragma comment(lib, "v8_base.lib")
#pragma comment(lib, "v8_libbase.lib")
#pragma comment(lib, "v8_nosnapshot.lib")
#pragma comment(lib, "v8_libplatform.lib")

int main()
{
#if defined(BLIB_WIN)
	HINSTANCE hInst = GetModuleHandle(0);
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_DATA1), "data");
	HGLOBAL hMem = LoadResource(NULL, hRes);
	DWORD size = SizeofResource(NULL, hRes);
	char* resText = (char*)LockResource(hMem);
	char* text = (char*)malloc(size + 1);
	memcpy(text, resText, size);
	text[size] = 0;

	int version = atoi(text);
	std::string appname = "Browedit 2.0, " + std::string(text);

	free(text);
	FreeResource(hMem);


#if !defined(_DEBUG)
	BT_InstallSehFilter();
	BT_SetAppName((appname.c_str()));
	BT_SetSupportEMail(("borfje@gmail.com"));
	BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL);
	BT_SetSupportServer(("borf.info"), 9999);
	BT_SetSupportURL(("http://browedit.excalibur-nw.com"));
#endif
#endif
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

#else
	Log::out<<"Loading assets/configs/config.linux.json"<<Log::newline;
	Json::Value linuxConfig = blib::util::FileSystem::getJson("assets/configs/config.linux.json");
	Log::out<<"Linux config size: "<<linuxConfig.size()<<Log::newline;
	mergeConfig(config, linuxConfig);
#endif


	if (config["moveconsole"].asBool())
		blib::util::fixConsole();




	v8::Isolate* isolate = v8::Isolate::New();
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handle_scope(isolate);
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);
	v8::Context::Scope context_scope(context);

	BrowEdit* app = new BrowEdit(config, isolate);
	app->version = version;
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
