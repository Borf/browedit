#include <BroLib/Rsw.h>
#include <fstream>
#include <blib/json.hpp>
#include <string>
#include <iostream>

#include <blib/util/Log.h>
#include <blib/util/FileSystem.h>
#include <BroLib/GrfFileSystemHandler.h>
#include <blib/platform/win32/Registry.h>
using blib::util::Log;

json vec3(const glm::vec3 &val)
{
	json ret;
	ret[0u] = val.x;
	ret[1u] = val.y;
	ret[2u] = val.z;
	return ret;
}
void mergeConfig(json& config, const json& newConfig)
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

std::string fixname(const std::string& name)
{
	std::string ret;
	for (int i = 0; i < name.size(); i++)
	{
		if ((unsigned char)name[i] < 128)
			ret += name[i];
		else
			ret += "\\u" + blib::util::Log::format("%04X", (unsigned char)name[i]);
	}
	return ret;
}


int main(int argc, char* argv[])
{
	std::string filename;
	if(argc > 1)
		filename = argv[1];
	if(filename.find(".") != std::string::npos)
		filename = filename.substr(0, filename.rfind("."));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(""));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(".."));

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
		for (const auto& file : configs)
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
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(config["data"]["ropath"].get<std::string>()));
	for (size_t i = 0; i < config["data"]["grfs"].size(); i++)
		blib::util::FileSystem::registerHandler(new GrfFileSystemHandler(config["data"]["grfs"][i].get<std::string>()));


	if (filename == "")
	{
		Log::out << "Listing all maps" << Log::newline;
		auto files = blib::util::FileSystem::getFileList([](const std::string& fileName)
			{
				if (fileName.size() < 4)
					return false;
				std::string ext = fileName.substr(fileName.length() - 4, 4);
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
				return ext == ".rsw";
			});
		std::sort(files.begin(), files.end());

		for (auto& file : files)
			std::cout << file << std::endl;

		return 0;
	}



	printf("Opening %s\n", filename.c_str());

	Rsw* rsw = new Rsw(filename, false);

	json data;

	data["version"] = rsw->version;
	data["inifile"] = rsw->iniFile;
	data["gndfile"] = rsw->gndFile;
	data["gatfile"] = rsw->gatFile;

	data["water"]["height"] = rsw->water.height;
	data["water"]["type"] = rsw->water.type.value;
	data["water"]["amplitude"] = rsw->water.amplitude;
	data["water"]["phase"] = rsw->water.phase;
	data["water"]["surfaceCurve"] = rsw->water.surfaceCurve;
	data["water"]["animSpeed"] = rsw->water.animSpeed;

	data["light"]["longitude"] = rsw->light.longitude;
	data["light"]["lattitude"] = rsw->light.latitude;
	data["light"]["diffuse"] = vec3(rsw->light.diffuse);
	data["light"]["ambient"]= vec3(rsw->light.ambient);
	data["light"]["intensity"] = rsw->light.intensity;

	for (int i = 0; i < 4; i++)
		data["unknown"][i] = rsw->unknown[i];


	for (size_t i = 0; i < rsw->objects.size(); i++)
	{
		json o;

		Rsw::Object* object = rsw->objects[i];
		switch (object->type)
		{
		case Rsw::Object::Type::Model: //1
		{
			o["type"] = "model";
			Rsw::Model* model = (Rsw::Model*)object;

			o["name"] = fixname(model->name);
			o["animType"] = model->animType;
			o["animSpeed"] = model->animSpeed;
			o["blockType"] = model->blockType;
			o["fileName"] = fixname(model->fileName);
			//unknown!!!!
			o["position"] = vec3(model->position);
			o["rotation"] = vec3(model->rotation);
			o["scale"] = vec3(model->scale);
		}
			break;
		case Rsw::Object::Type::Light:	//2 Light
		{
			o["type"] = "light";
			Rsw::Light* light = (Rsw::Light*)object;
			o["name"] = fixname(light->name);
			o["position"] = vec3(light->position);
			o["todo"] = light->todo; 
			o["color"] = vec3(light->color);
			o["todo2"] = light->todo2;
		}
			break;
		case Rsw::Object::Type::Sound://3: //Sound
		{
			o["type"] = "sound";
			Rsw::Sound* sound = (Rsw::Sound*)object;
			o["name"] = fixname(sound->name);
			o["filename"] = fixname(sound->fileName);
			o["position"] = vec3(sound->position);
			o["volume"] = sound->vol;
			o["width"] = sound->width;
			o["height"] = sound->height;
			o["range"] = sound->range;
//			if (rsw->version >= 0x0200)
			o["cycle"] = sound->cycle;
		}
			break;
		case Rsw::Object::Type::Effect://4: //Effect
		{
			o["type"] = "effect";
			Rsw::Effect* effect = (Rsw::Effect*)object;
			o["name"] = fixname(effect->name);
			o["position"] = vec3(effect->position);
			o["id"] = effect->id; 
			o["loop"] = effect->loop;
			o["param1"] = effect->param1;
			o["param2"] = effect->param2;
			o["param3"] = effect->param3;
			o["param4"] = effect->param4;
		}
			break;
		}
		data["objects"].push_back(o);
	}


	std::ofstream out((filename + ".json").c_str(), std::ios_base::out | std::ios_base::binary);
	out<< data;
	out.close();
	printf("Wrote to %s\n", (filename + ".json").c_str());
	return 0;
}