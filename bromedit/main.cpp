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

#include "BromEdit.h"
#include <BroLib/GrfFileSystemHandler.h>
using blib::util::Log;


#ifndef _DEBUG
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "wininet.lib")
#endif

void mergeConfig(json &config, const json &newConfig);


extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main(int argc, char* argv[])
{
#if defined(BLIB_WIN)
	std::string appname = "Browedit 2.0";
	int version = 999;
#endif
	blib::util::Thread::setMainThread();
	Log::out << "/============================================================\\" << Log::newline;
	Log::out << "||   ____                          ______     _  _  _       ||" << Log::newline;
	Log::out << "||  |  _ \\                        |  ____|   | |(_)| |      ||" << Log::newline;
	Log::out << "||  | |_) | _ __  ___   _ __ ___  | |__    __| | _ | |_     ||" << Log::newline;
	Log::out << "||  |  _ < | '__|/ _ \\ | '_ ` _ \\ |  __|  / _` || || __|    ||" << Log::newline;
	Log::out << "||  | |_) || |  | (_) || | | | | || |____| (_| || || |_     ||" << Log::newline;
	Log::out << "||  |____/ |_|   \\___/ |_| |_| |_||______|\\__,_||_| \\__|    ||" << Log::newline;
	Log::out << "||                                                          ||" << Log::newline;
	Log::out << "||                   Coded by:      Borf                    ||" << Log::newline;
	Log::out << "||                                                          ||" << Log::newline;
	Log::out << "||                     Libraries used:                      ||" << Log::newline;
	Log::out << "||            - OpenGL                                      ||" << Log::newline;
	Log::out << "||            - blib                                        ||" << Log::newline;
	Log::out << "||            - zlib                                        ||" << Log::newline;
	Log::out << "||            - grflib                                      ||" << Log::newline;
	Log::out << "||            - glew                                        ||" << Log::newline;
	Log::out << "||            - glm                                         ||" << Log::newline;
	Log::out << "||            - jsoncpp                                     ||" << Log::newline;
	Log::out << "||            - stb_image                                   ||" << Log::newline;
	Log::out << "||            - (win)sockets                                ||" << Log::newline;
	Log::out << "||                                                          ||" << Log::newline;
	Log::out << "\\============================================================/" << Log::newline;
	Log::out << "" << Log::newline;
	Log::out << "" << Log::newline;
	Log::out << "       Not really a release, don't bug about any bugs..." << Log::newline;
	Log::out << Log::newline;

	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(""));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(".."));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler("../blib"));

	Log::out << "Loading configuration..." << Log::newline;
	json config = blib::util::FileSystem::getJson("assets/configs/config.default.json");
#ifdef WIN32
	blib::platform::win32::RegistryKey key(HKEY_CURRENT_USER, "Software\\Browedit");
	if (!key.exists() && !key.create()) {
		// Something went wrong
	}
	blib::platform::win32::RegistryValue value(key, "config");
	std::string configFileName = value.readString("");
	while (!blib::util::FileSystem::exists("assets/configs/" + configFileName))
	{
		// we ask him for his choice
		Log::err << "Unable to find configuration file, please type the configuration filename" << Log::newline;
		std::vector<std::string> configs = blib::util::FileSystem::getFileList("assets/configs");
		for (const auto &file : configs)
			if (file[0] != '.')
				Log::err << " - " << file << Log::newline;
		std::getline(std::cin, configFileName);
	}
	value.writeString(configFileName);

	mergeConfig(config, blib::util::FileSystem::getJson("assets/configs/" + configFileName));

#else
	Log::out << "Loading assets/configs/config.linux.json" << Log::newline;
	json linuxConfig = blib::util::FileSystem::getJson("assets/configs/config.linux.json");
	Log::out << "Linux config size: " << linuxConfig.size() << Log::newline;
	mergeConfig(config, linuxConfig);
#endif


	if (config["moveconsole"].get<bool>())
		blib::util::fixConsole();


	BromEdit* app = new BromEdit(config);
#ifdef FIXME
	//if (argc > 1)
	//	app->model = new IRsm(argv[1]);
#endif
	app->start();
	delete app;
	blib::util::FileSystem::dispose();
	return 0;
}



void mergeConfig(json &config, const json &newConfig)
{
	for (auto it = newConfig.begin(); it != newConfig.end(); it++)
		if (config.find(it.key()) != config.end())
			if (config[it.key()].is_object())
				mergeConfig(config[it.key()], it.value());
			else
				config[it.key()] = it.value();
		else
			config[it.key()] = it.value();
}
