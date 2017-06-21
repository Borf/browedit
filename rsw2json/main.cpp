#include <BroLib/Rsw.h>
#include <fstream>
#include <blib/json.hpp>
#include <string>

#include <blib/util/FileSystem.h>


json vec3(const glm::vec3 &val)
{
	json ret;
	ret[0u] = val.x;
	ret[1u] = val.y;
	ret[2u] = val.z;
	return ret;
}

int main(int argc, char* argv[])
{
	std::string filename = argv[1];
	filename = filename.substr(0, filename.rfind("."));


	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(""));

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

			o["name"] = model->name;
			o["animType"] = model->animType;
			o["animSpeed"] = model->animSpeed;
			o["blockType"] = model->blockType;
			o["fileName"] = model->fileName;
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
			o["name"] = light->name;
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
			o["name"] = sound->name; 
			o["filename"] = sound->fileName;
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
			o["name"] = effect->name;
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


	std::ofstream((filename + ".json").c_str(), std::ios_base::out | std::ios_base::binary) << data;

	printf("Wrote to %s\n", (filename + ".json").c_str());
	return 0;
}