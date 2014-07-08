#include "Rsw.h"
#include "Rsm.h"
#include "MapRenderer.h"

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

				model->model = getRsm(model->fileName);
				objects.push_back(model);
			}
			break;
		case 2:	//Light
			//			RswLight* light = new RswLight();
			{
				Light* light = new Light();
				light->name = file->readString(40);
				light->position = file->readVec3();
				light->todo = file->readString(40);
				light->color = file->readVec3();
				light->todo2 = file->readFloat();

				objects.push_back(light);
			}
			//file->readString(40 + 12 + 40 + 12 + 4);
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
		{
			Effect* effect = new Effect();
			effect->name = file->readString(80);
			effect->position = file->readVec3();
			effect->type = file->readInt();
			effect->loop = file->readFloat();
			effect->param1 = file->readFloat();
			effect->param2 = file->readFloat();
			effect->param3 = file->readFloat();
			effect->param4 = file->readFloat();
			objects.push_back(effect);

		}
//			file->readString(80 + 12 + 4 + 4 + 4 + 4 + 4 + 4);
			break;
		default:
			Log::out<<"Unknown object type in rsw file: "<<objectType<<Log::newline;
			break;
		}
	}


	while(!file->eof())
		quadtreeFloats.push_back(file->readVec3());
	quadtree = new QuadTreeNode(quadtreeFloats.begin());

	delete file;
}



void Rsw::save(const std::string &fileName)
{
	blib::util::PhysicalFileSystemHandler::StreamOutFilePhysical* pFile = new blib::util::PhysicalFileSystemHandler::StreamOutFilePhysical(fileName + ".rsw");

	char header[5] = "GRSW";
	pFile->write(header, 4);
	pFile->writeShort(version);

	pFile->writeString(iniFile, 40);
	pFile->writeString(gndFile, 40);
	if (version > 0x0104)
		pFile->writeString(gatFile, 40);

	pFile->writeString(iniFile, 40);

	//TODO: default values
	if (version >= 0x103)
		pFile->writeFloat(water.height);
	if (version >= 0x108)
	{
		pFile->writeInt(water.type);
		pFile->writeFloat(water.amplitude);
		pFile->writeFloat(water.phase);
		pFile->writeFloat(water.surfaceCurve);
	}
	if (version >= 0x109)
		pFile->writeInt(water.animSpeed);
	else
	{
		throw "todo";
	}

	if (version >= 0x105)
	{
		pFile->writeInt(light.longitude);
		pFile->writeInt(light.lattitude);
		pFile->writeVec3(light.diffuse);
		pFile->writeVec3(light.ambient);
	}
	if (version >= 0x107)
		pFile->writeFloat(light.intensity);


	if (version >= 0x106)
	{
		pFile->writeInt(unknown[0]);
		pFile->writeInt(unknown[1]);
		pFile->writeInt(unknown[2]);
		pFile->writeInt(unknown[3]);
	}
	else
	{
		unknown[0] = unknown[2] = -500;
		unknown[1] = unknown[3] = 500;
	}

	pFile->writeInt(objects.size());
	for (size_t i = 0; i < objects.size(); i++)
	{
		Object* object = objects[i];
		switch (object->type)
		{
		case Object::Type::Model: //1
		{
			pFile->writeInt(1);
			Model* model = (Model*)object;
			if (version >= 0x103)
			{
				pFile->writeString(model->name, 40);
				pFile->writeInt(model->animType);
				pFile->writeFloat(model->animSpeed);
				pFile->writeInt(model->blockType);
			}
			pFile->writeString(model->fileName, 80);
			pFile->writeString("", 80); // TODO: Unknown?
			pFile->writeVec3(model->position);
			pFile->writeVec3(model->rotation);
			pFile->writeVec3(model->scale);
		}
			break;
		case Object::Type::Light:	//2 Light
		{
			pFile->writeInt(2);
			Light* light = (Light*)object;
			pFile->writeString(light->name, 40);
			pFile->writeVec3(light->position);
			pFile->writeString(light->todo, 40);
			pFile->writeVec3(light->color);
			pFile->writeFloat(light->todo2);
		}
			break;
		case Object::Type::Sound://3: //Sound
		{
			pFile->writeInt(3);
			Sound* sound = new Sound();
			pFile->writeString(sound->name, 80);
			pFile->writeString(sound->fileName, 80);
			pFile->writeVec3(sound->position);
			pFile->writeFloat(sound->vol);
			pFile->writeInt(sound->width);
			pFile->writeInt(sound->height);
			pFile->writeFloat(sound->range);
			if (version >= 0x0200)
				pFile->writeFloat(0); //cycle
		}
			break;
		case Object::Type::Effect://4: //Effect
		{
			pFile->writeInt(4);
			Effect* effect = (Effect*)object;
			pFile->writeString(effect->name, 80);
			pFile->writeVec3(effect->position);
			pFile->writeInt(effect->type);
			pFile->writeFloat(effect->loop);
			pFile->writeFloat(effect->param1);
			pFile->writeFloat(effect->param2);
			pFile->writeFloat(effect->param3);
			pFile->writeFloat(effect->param4);
		}
			break;
		default:
			Log::out << "Unknown object type in rsw file: "<< Log::newline;
			break;
		}

	}

	for (size_t i = 0; i < quadtreeFloats.size(); i++)
		pFile->writeVec3(quadtreeFloats[i]);




	delete pFile;
}




Rsm* Rsw::getRsm( const std::string &fileName )
{
	std::map<std::string, Rsm*>::iterator it = rsmCache.find(fileName);
	if (it == rsmCache.end())
	{
		Rsm* rsm = new Rsm("data/model/" + fileName);
		Log::out << "Rsw: loading mesh " << fileName << Log::newline;
		rsmCache[fileName] = rsm->loaded ? rsm : NULL;
		if (!rsm->loaded)
			delete rsm;
		it = rsmCache.find(fileName);
	}
	else
		Log::out << "Rsw: cache hit for mesh " << fileName << Log::newline;
	return it->second;
}


Rsw::Model::~Model()
{

}


bool collides_(Rsm::Mesh* mesh, const blib::math::Ray &ray, glm::mat4 matrix)
{
	glm::mat4 newMatrix = matrix * mesh->renderer->matrix;
	newMatrix = glm::inverse(newMatrix);
	blib::math::Ray newRay = ray * newMatrix;


	std::vector<glm::vec3> verts;
	verts.resize(3);
	float t;
	for (size_t i = 0; i < mesh->faces.size(); i++)
	{
		for (size_t ii = 0; ii < 3; ii++)
			verts[ii] = mesh->vertices[mesh->faces[i]->vertices[ii]];// glm::vec3(matrix * mesh->renderer->matrix * glm::vec4(mesh->vertices[mesh->faces[i]->vertices[ii]], 1));

		if (newRay.LineIntersectPolygon(verts, t))
			return true;
	}

	for (size_t i = 0; i < mesh->children.size(); i++)
	{
		if (collides_(mesh->children[i], ray, matrix))
			return true;
	}


	return false;
}

bool Rsw::Model::collides(const blib::math::Ray &ray)
{
	if (!aabb.hasRayCollision(ray, 0, 10000000))
		return false;

	return collides_(model->rootMesh, ray, matrixCache);
	return true;
}

Rsw::QuadTreeNode::QuadTreeNode(std::vector<glm::vec3>::iterator &it, int level /*= 0*/) : bbox(glm::vec3(0, 0, 0), glm::vec3(0,0,0))
{
	bbox.bounds[0] = *it;
	it++;
	bbox.bounds[1] = *it;
	it++;

	range[0] = *it;
	it++;
	range[1] = *it;
	it++;
	
	if (level >= 5)
		return;
	for (size_t i = 0; i < 4; i++)
		children[i] = new QuadTreeNode(it, level + 1);
	
}
