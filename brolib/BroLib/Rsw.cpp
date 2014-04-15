#include "Rsw.h"
#include "Rsm.h"

#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>


using blib::util::Log;

Rsw::Rsw(const std::string &fileName)
{
	char header[4];
	blib::util::StreamInFile* file = new blib::util::StreamInFile(fileName + ".rsw");
	file->read(header, 4);
	if(header[0] == 'G' && header[1] == 'R' && header[2] == 'G' && header[3] == 'W')
	{
		Log::out<<"Error loading rsw: invalid header"<<Log::newline;
		delete file;
		return;
	}
	version = file->readShort();


	iniFile = file->readString(40);
	gndFile = file->readString(40);
	if(version > 0x0104)
		gatFile = file->readString(40);
	else
		gatFile = gndFile; //TODO: convert

	iniFile = file->readString(40); // ehh...read inifile twice?

	//TODO: default values
	if(version >= 0x103)
		water.height = file->readFloat();
	if(version >= 0x108)
	{
		water.type = file->readInt();
		water.amplitude = file->readFloat();
		water.phase = file->readFloat();
		water.surfaceCurve = file->readFloat();
	}
	if(version >= 0x109)
		water.animSpeed = file->readInt();
	else
	{
		water.animSpeed = 100;
		throw "todo";
	}


	light.longitude = 45;//TODO: remove the defaults here and put defaults of the water somewhere too
	light.lattitude = 45;
	light.diffuse = glm::vec3(1,1,1);
	light.ambient = glm::vec3(0.3f,0.3f,0.3f);
	light.intensity = 0.5f;

	if(version >= 0x105)
	{
		light.longitude = file->readInt();
		light.lattitude = file->readInt();
		light.diffuse = file->readVec3();
		light.ambient = file->readVec3();
	}
	if(version >= 0x107)
		light.intensity = file->readFloat();


	if(version >= 0x106)
	{
		unknown[0] = file->readInt();
		unknown[1] = file->readInt();
		unknown[2] = file->readInt();
		unknown[3] = file->readInt();
	}
	else
	{
		unknown[0] = unknown[2] = -500;
		unknown[1] = unknown[3] = 500;
	}


	int objectCount = file->readInt();
	for(int i = 0; i < objectCount; i++)
	{
		int objectType = file->readInt();
		switch(objectType)
		{
		case 1:	//Model
			{
				Model* model = new Model();
				model->matrixCached = false;
				if(version >= 0x103)
				{
					model->name = file->readString(40);
					model->animType = file->readInt();
					model->animSpeed = file->readFloat();
					model->blockType = file->readInt();
				}
				model->fileName = file->readString(80);
				file->readString(80); // TODO: Unknown?
				model->position = file->readVec3();
				model->rotation = file->readVec3();
				model->scale = file->readVec3();

				model->model = getRsw(model->fileName);
				objects.push_back(model);
			}
			break;
		case 2:	//Light
			//			RswLight* light = new RswLight();
			file->readString(40 + 12 + 40 + 12 + 4);
			break;
		case 3: //Sound
			{
				Sound* sound = new Sound();
				sound->name = file->readString(80);
				sound->fileName = file->readString(80);
				sound->position = file->readVec3();
				sound->vol = file->readFloat();
				sound->width = file->readInt();
				sound->height = file->readInt();
				sound->range = file->readFloat();
//				file->readString(80 + 80 + 12 + 4 + 4 + 4 + 4);
				if (version >= 0x0200)
					file->readFloat(); // cycle
				objects.push_back(sound);
			}
			break;
		case 4: //Effect
			file->readString(80 + 12 + 4 + 4 + 4 + 4 + 4 + 4);
			break;
		default:
			Log::out<<"Unknown object type in rsw file: "<<objectType<<Log::newline;
			break;
		}
	}


	std::vector<float> quadtreeFloats;
	while(!file->eof())
		quadtreeFloats.push_back(file->readFloat());

	delete file;



}

Rsm* Rsw::getRsw( const std::string &fileName )
{
	std::map<std::string, Rsm*>::iterator it = rsmCache.find(fileName);
	if(it == rsmCache.end())
	{
		Rsm* rsm = new Rsm("data/model/" + fileName);
		rsmCache[fileName] = rsm->loaded ? rsm : NULL;
		if(!rsm->loaded)
			delete rsm;
		it = rsmCache.find(fileName);
	}	
	return it->second;
}

Rsw::Model::~Model()
{

}
