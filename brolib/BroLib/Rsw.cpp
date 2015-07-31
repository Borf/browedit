#include "Rsw.h"
#include "Rsm.h"
#include "Gnd.h"
#include "MapRenderer.h"
#include <blib/util/Log.h>
#include <blib/Util.h>
#include <blib/linq.h>
using blib::util::Log;

#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>

#define assert2(expr) if(!(expr)) { Log::out<<__FILE__<<":"<<__LINE__<<"\n Debug assertion failed: " #expr ""<<Log::newline; }
#define assert3(expr, msg) if(!(expr)) { Log::out<<__FILE__<<":"<<__LINE__<<"\n\t-> Debug assertion failed: " #expr ": "<<(msg)<<Log::newline; }

using blib::util::Log;

Rsw::Rsw(int width, int height)
{
	version = 0x0201;
	water.height = 1;
	water.type = 0;
	water.amplitude = 1.0f;
	water.phase = 2.0f;
	water.surfaceCurve = 50.0f;
	water.animSpeed = 3;

	light.longitude = 45;
	light.latitude = 45;
	light.diffuse = glm::vec3(1, 1, 1);
	light.ambient = glm::vec3(0.55f, 0.55f, 0.55f);
	light.intensity = 0.5f;
	
	//dunno, wtf is this?
	unknown[0] = 9;
	unknown[1] = 34004;
	unknown[2] = 512;
	unknown[3] = 0;


	std::vector<glm::vec3> data;
	std::function<void(int, float, float, float, float)> fill;
	fill = [&data, &fill](int level, float width, float height, float centerx, float centery)
	{
		glm::vec3 bbmin(centerx - width * 5, 0, centerx - height * 5);
		glm::vec3 bbmax(centerx + width * 5, 0, centerx + height * 5);
		glm::vec3 range((bbmax - bbmin) / 2.0f);
		data.push_back(bbmin);
		data.push_back(bbmax);
		data.push_back(range);
		data.push_back(bbmax - range);

		if (level < 5)
		{
			fill(level + 1, width / 2, height / 2, centerx - width*2.5f, centery - height*2.5f);
			fill(level + 1, width / 2, height / 2, centerx - width*2.5f, centery + height*2.5f);
			fill(level + 1, width / 2, height / 2, centerx + width*2.5f, centery + height*2.5f);
			fill(level + 1, width / 2, height / 2, centerx + width*2.5f, centery - height*2.5f);
		}
	};
	
	fill(0, (float)width, (float)height, 0.0f, 0.0f);



	quadtree = new QuadTreeNode(data.cbegin());



}

Rsw::Rsw(const std::string &fileName, bool loadModels)
{
	char header[4];
	blib::util::StreamInFile* file = new blib::util::StreamInFile(fileName + ".rsw");
	file->read(header, 4);
	if(header[0] == 'G' && header[1] == 'R' && header[2] == 'G' && header[3] == 'W')
	{
		Log::out<<"RSW: Error loading rsw: invalid header"<<Log::newline;
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
	light.latitude = 45;
	light.diffuse = glm::vec3(1,1,1);
	light.ambient = glm::vec3(0.3f,0.3f,0.3f);
	light.intensity = 0.5f;

	if(version >= 0x105)
	{
		light.longitude = file->readInt();
		light.latitude = file->readInt();
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
				if (loadModels)
					model->model = getRsm(model->fileName);
				else
					model->model = NULL;
				objects.push_back(model);
			}
			break;
		case 2:	//Light
			//			RswLight* light = new RswLight();
			{
				Light* light = new Light();
				light->name = file->readString(40);
				light->todo = file->readString(40);
				light->position = file->readVec3();
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
					sound->cycle = file->readFloat(); // cycle
				objects.push_back(sound);
			}
			break;
		case 4: //Effect
		{
			Effect* effect = new Effect();
			effect->name = file->readString(80);
			effect->position = file->readVec3();
			effect->id = file->readInt();
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
	quadtree = new QuadTreeNode(quadtreeFloats.cbegin());

	delete file;
}


Rsw::~Rsw()
{
	delete quadtree;
	blib::linq::deleteall(objects);

	for (auto rsm : rsmCache)
		delete rsm.second;
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
		pFile->writeInt(light.latitude);
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
			pFile->writeString(light->todo, 40);
			pFile->writeVec3(light->position);
			pFile->writeVec3(light->color);
			pFile->writeFloat(light->todo2);
		}
			break;
		case Object::Type::Sound://3: //Sound
		{
			pFile->writeInt(3);
			Sound* sound = (Sound*)object;
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
			pFile->writeInt(effect->id);
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
		//Log::out << "Rsw: loading mesh " << fileName << Log::newline;
		rsmCache[fileName] = rsm->loaded ? rsm : NULL;
		if (!rsm->loaded)
			delete rsm;
		it = rsmCache.find(fileName);
	}
	//else
	//	Log::out << "Rsw: cache hit for mesh " << fileName << Log::newline;
	return it->second;
}

void Rsw::recalculateQuadTree(Gnd* gnd)
{
#define MAP_MIN -9999999
#define MAP_MAX 9999999
	Log::out << "Recalculating quadtree" << Log::newline;

	//first build up a grid, based on the gnd grid
	std::vector<std::vector<glm::vec2>> heights(gnd->width, std::vector<glm::vec2>(gnd->height, glm::vec2(MAP_MAX,MAP_MIN)));
	for (int x = 0; x < gnd->width; x++)
	{
		for (int y = 0; y < gnd->height; y++)
		{
			heights[x][y].x = glm::min(glm::min(glm::min(gnd->cubes[x][y]->h1, gnd->cubes[x][y]->h2), gnd->cubes[x][y]->h3), gnd->cubes[x][y]->h4);
			heights[x][y].y = glm::max(glm::max(glm::max(gnd->cubes[x][y]->h1, gnd->cubes[x][y]->h2), gnd->cubes[x][y]->h3), gnd->cubes[x][y]->h4);
		}
	}

	//loop through all polygons of all objects and then adjust the min/max per tile
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->type == Object::Type::Model)
		{
			((Model*)objects[i])->foreachface([gnd, &heights](const std::vector<glm::vec3> &verts) {
				for (glm::vec3 vert : verts)
				{
					int x = (int)(vert.x / 10);
					int y = (int)(gnd->height - (vert.z / 10));
					if (x < 0 || x >= gnd->width || y < 0 || y >= gnd->height)
						continue;
					heights[x][y].x = glm::min(heights[x][y].x, -vert.y);
					heights[x][y].y = glm::max(heights[x][y].y, -vert.y);
				}
			});
		}
	}

	std::vector<QuadTreeNode*> nodes;
	quadtree->foreachLevel([&nodes](QuadTreeNode* node, int level) {	if (level == 5) nodes.push_back(node);	});
	int index = 0;
	for (QuadTreeNode* node : nodes)
	{
		assert3(node->range[0].x - (node->bbox.max.x - node->bbox.min.x) / 2 < 0.000001, "Difference: " + blib::util::toString(node->range[0].x - (node->bbox.max.x - node->bbox.min.x) / 2) + " (should be small)");
		assert3(node->range[0].y - (node->bbox.max.y - node->bbox.min.y) / 2 < 0.000001, "Difference: " + blib::util::toString(node->range[0].y - (node->bbox.max.y - node->bbox.min.y) / 2) + " (should be small)");
		assert3(node->range[0].z - (node->bbox.max.z - node->bbox.min.z) / 2 < 0.000001, "Difference: " + blib::util::toString(node->range[0].z - (node->bbox.max.z - node->bbox.min.z) / 2) + " (should be small)");
		assert3(node->range[1].x - (node->bbox.max.x - node->range[0].x < 0.000001), "Difference: " + blib::util::toString(node->range[1].x - (node->bbox.max.x - node->range[0].x)) + " (should be small)");
		assert3(node->range[1].y - (node->bbox.max.y - node->range[0].y < 0.000001), "Difference: " + blib::util::toString(node->range[1].x - (node->bbox.max.x - node->range[0].x)) + " (should be small)");
		assert3(node->range[1].z - (node->bbox.max.z - node->range[0].z < 0.000001), "Difference: " + blib::util::toString(node->range[1].x - (node->bbox.max.x - node->range[0].x)) + " (should be small)");


		node->bbox.min.y = MAP_MAX;
		node->bbox.max.y = MAP_MIN;
		for (float x = node->bbox.min.x; x <= node->bbox.max.x; x += 1)
		{
			for (float z = node->bbox.min.z; z <= node->bbox.max.z; z += 1)
			{
				int xx = (int)floor((gnd->width * 5 + x) / 10);
				int yy = (int)floor((gnd->height * 5 + z) / 10);
				int xxx = (int)ceil((gnd->width * 5 + x) / 10);
				int yyy = (int)ceil((gnd->height * 5 + z) / 10);
				if (xx < 0 || yy < 0 || xx >= gnd->width || yy >= gnd->height ||
					xxx < 0 || yyy < 0 || xxx >= gnd->width || yyy >= gnd->height)
					continue;

				if (heights[xx][yy].x != MAP_MAX &&
					heights[xx][yy].y != MAP_MIN)
				{
				node->bbox.min.y = glm::min(node->bbox.min.y, heights[xx][yy].x);
				node->bbox.max.y = glm::max(node->bbox.max.y, heights[xx][yy].y);
			}
				if (heights[xxx][yy].x != MAP_MAX &&
					heights[xxx][yy].y != MAP_MIN)
				{
					node->bbox.min.y = glm::min(node->bbox.min.y, heights[xxx][yy].x);
					node->bbox.max.y = glm::max(node->bbox.max.y, heights[xxx][yy].y);
		}
				if (heights[xx][yyy].x != MAP_MAX &&
					heights[xx][yyy].y != MAP_MIN)
				{
					node->bbox.min.y = glm::min(node->bbox.min.y, heights[xx][yyy].x);
					node->bbox.max.y = glm::max(node->bbox.max.y, heights[xx][yyy].y);
				}
				if (heights[xxx][yyy].x != MAP_MAX &&
					heights[xxx][yyy].y != MAP_MIN)
				{
					node->bbox.min.y = glm::min(node->bbox.min.y, heights[xxx][yyy].x);
					node->bbox.max.y = glm::max(node->bbox.max.y, heights[xxx][yyy].y);
				}
			}
		}
		if (node->bbox.min.y == MAP_MAX)
			node->bbox.min.y = 0;
		if (node->bbox.max.y = MAP_MIN)
			node->bbox.max.y = 0;

		node->range[0] = (node->bbox.max - node->bbox.min) / 2.0f;
		node->range[1] = node->bbox.max - node->range[0];
		index++;
	}

	for (int i = 4; i >= 0; i--)
	{
		quadtree->foreachLevel([&nodes, i](QuadTreeNode* node, int level) 
		{
			if (level == i)
			{
				node->bbox.min.y = MAP_MAX;
				node->bbox.max.y = MAP_MIN;

				for (int ii = 0; ii < 4; ii++)
				{
					node->bbox.min.y = glm::min(node->bbox.min.y, node->children[ii]->bbox.min.y);
					node->bbox.max.y = glm::max(node->bbox.max.y, node->children[ii]->bbox.max.y);
				}
				node->range[0] = (node->bbox.max - node->bbox.min) / 2.0f;
				node->range[1] = node->bbox.max - node->range[0];
			}
		});
	}
	Log::out << "Done recalculating quadtree" << Log::newline;
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

std::vector<glm::vec3> collisions_(Rsm::Mesh* mesh, const blib::math::Ray &ray, glm::mat4 matrix)
{
	std::vector<glm::vec3> ret;

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
		{
			ret.push_back(glm::vec3(matrix * mesh->renderer->matrix * glm::vec4(newRay.origin + t * newRay.dir, 1)));
		}
	}

	for (size_t i = 0; i < mesh->children.size(); i++)
	{
		std::vector<glm::vec3> other = collisions_(mesh->children[i], ray, matrix);
		if (!other.empty())
			ret.insert(ret.end(), other.begin(), other.end());
	}
	return ret;
}

bool Rsw::Model::collides(const blib::math::Ray &ray)
{
	if (!aabb.hasRayCollision(ray, 0, 10000000))
		return false;

	return collides_(model->rootMesh, ray, matrixCache);
	return true;
}

std::vector<glm::vec3> Rsw::Model::collisions(const blib::math::Ray &ray)
{
	if (!aabb.hasRayCollision(ray, 0, 10000000))
		return std::vector<glm::vec3>();

	return collisions_(model->rootMesh, ray, matrixCache);
}


void foreachface_(Rsm::Mesh* mesh, std::function<void(const std::vector<glm::vec3>&)> callback, glm::mat4 matrix)
{
	glm::mat4 newMatrix = matrix * mesh->renderer->matrix;


	std::vector<glm::vec3> verts;
	verts.resize(3);
	for (size_t i = 0; i < mesh->faces.size(); i++)
	{
		for (size_t ii = 0; ii < 3; ii++)
			verts[ii] = glm::vec3(matrix * mesh->renderer->matrix * glm::vec4(mesh->vertices[mesh->faces[i]->vertices[ii]], 1));
		callback(verts);
	}

	for (size_t i = 0; i < mesh->children.size(); i++)
		foreachface_(mesh->children[i], callback, matrix);
}


void Rsw::Model::foreachface(std::function<void(const std::vector<glm::vec3>&)> callback)
{
	if (!model)
		return;
	foreachface_(model->rootMesh, callback, matrixCache);
}

Rsw::QuadTreeNode::QuadTreeNode(std::vector<glm::vec3>::const_iterator &it, int level /*= 0*/) : bbox(glm::vec3(0, 0, 0), glm::vec3(0,0,0))
{
	bbox.bounds[1] = *it;
	it++;
	bbox.bounds[0] = *it;
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

Rsw::QuadTreeNode::~QuadTreeNode()
{
	for (int i = 0; i < 4; i++)
		if (children[i])
			delete children[i];
}
