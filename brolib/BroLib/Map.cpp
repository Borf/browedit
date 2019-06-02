#include "Map.h"
#include "Gnd.h"
#include "Rsw.h"
#include "Gat.h"

#include <blib/util/stb_image_create.h>
#include <blib/util/stb_image.h>
#include <blib/BackgroundTask.h>
#include <blib/util/Log.h>
using blib::util::Log;

#include <fstream>



Map::Map( const std::string &fileName )
{
	this->fileName = fileName;
	Log::out<<"Map: loading map "<<fileName<<Log::newline;

	//TODO: for some reason it doesn't like to parallalize these tasks...investigate later
#if 0
	blib::BackgroundTask<int>* gndTask = new blib::BackgroundTask<int>(NULL, [this, fileName]() { gnd = new Gnd(fileName); return 0; });
	blib::BackgroundTask<int>* rswTask = new blib::BackgroundTask<int>(NULL, [this, fileName]() { rsw = new Rsw(fileName); return 0; });
	blib::BackgroundTask<int>* gatTask = new blib::BackgroundTask<int>(NULL, [this, fileName]() { gat = new Gat(fileName); return 0; });
	gndTask->waitForTermination();
	rswTask->waitForTermination();
	gatTask->waitForTermination();
#else
	gnd = new Gnd(fileName);
	rsw = new Rsw(fileName);
	gat = new Gat(fileName);
#endif
	Log::out<<"Map: Done loading map"<<Log::newline;
}


Map::Map(const std::string &fileName, int width, int height)
{
	Log::out << "Map: New map " << fileName << " of " << width << "x" << height << Log::newline;
	this->fileName = fileName;

	gnd = new Gnd(width, height);
	rsw = new Rsw(width, height);
	gat = new Gat(width*2, height*2);
}

Map::~Map()
{
	delete gnd;
	delete rsw;
	delete gat;
	gnd = NULL;
	rsw = NULL;
	gat = NULL;
}


void Map::save(const std::string &filename)
{
	if (filename != "")
		this->fileName = filename;

	gnd->save(this->fileName);
	rsw->recalculateQuadTree(gnd);
	rsw->save(this->fileName);
	gat->save(this->fileName);
}

bool Map::inMap(int x, int y)
{
	return x >= 0 && x < gnd->width && y >= 0 && y < gnd->height;
}

void Map::saveHeightmap( const std::string &fileName )
{
	char* data = new char[(gnd->width * 2 )  * (gnd->height * 2) * 3];
	float minHeight = 999999;
	float maxHeight = -999999;
	for (int y = 0; y < gnd->height; y++)
	{
		for (int x = 0; x < gnd->width; x++)
		{
			for (int i = 0; i < 4; i++)
			{
				minHeight = glm::min(minHeight, gnd->cubes[x][y]->heights[i]);
				maxHeight = glm::max(maxHeight, gnd->cubes[x][y]->heights[i]);
			}
		}
	}

	for (int y = 0; y < gnd->height; y++)
	{
		for (int x = 0; x < gnd->width; x++)
		{
			for (int i = 0; i < 4; i++)
			{
				int xx = i % 2;
				int yy = i / 2;
				int index = ((2 * x + xx) + (gnd->width * 2) * (2 * y + yy)) * 3;
				for (int ii = 0; ii < 3; ii++)
					data[index+ii] = (unsigned char)((gnd->cubes[x][y]->heights[i] - minHeight) * (255 / (maxHeight - minHeight)));
			}
		}
	}

	stbi_write_png(fileName.c_str(), gnd->width * 2, gnd->height * 2, 3, data, 0);
	delete[] data;
}

void Map::loadHeightmap(const std::string &fileName)
{
	int width, height, comp;

	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &comp, 3);
	if (!data || width != gnd->width * 2 || height != gnd->height * 2)
	{
		if (data)
			stbi_image_free(data);
		return;
	}


	heightImportCubes.resize(width/2, std::vector<Gnd::Cube>(height/2));

	float minHeight = 999999;
	float maxHeight = -999999;
	for (int y = 0; y < gnd->height; y++)
	{
		for (int x = 0; x < gnd->width; x++)
		{
			for (int i = 0; i < 4; i++)
			{
				minHeight = glm::min(minHeight, gnd->cubes[x][y]->heights[i]);
				maxHeight = glm::max(maxHeight, gnd->cubes[x][y]->heights[i]);
			}
		}
	}

	for (int y = 0; y < gnd->height; y++)
	{
		for (int x = 0; x < gnd->width; x++)
		{
			for (int i = 0; i < 4; i++)
			{
				int xx = i % 2;
				int yy = i / 2;
				int index = ((2 * x + xx) + (gnd->width * 2) * (2 * y + yy)) * 3;
				heightImportCubes[x][y].heights[i] = data[index] / 255.0f;
			}
		}
	}

	heightImportMin = 350;
	heightImportMax = 1050;

	stbi_image_free(data);
}


void Map::exportLightmap(const std::string &fileName)
{
	char* data = new char[(gnd->width * 6)  * (gnd->height * 6) * 3];

	for (int y = 0; y < gnd->height; y++)
	{
		for (int x = 0; x < gnd->width; x++)
		{
			for (int xx = 0; xx < 6; xx++)
			{
				for (int yy = 0; yy < 6; yy++)
				{
					for (int i = 0; i < 4; i++)
					{
						int brightness = gnd->getLightmapBrightness(x, y, xx + 1, yy + 1);

						int index = ((6 * x + xx) + (gnd->width * 6) * (6 * y + yy)) * 3;
						for (int ii = 0; ii < 3; ii++)
							data[index + ii] = brightness;
					}
				}
			}
		}
	}

	stbi_write_png(fileName.c_str(), gnd->width * 6, gnd->height * 6, 3, data, 0);
	delete[] data;
}

void Map::exportObj(const std::string &fileName)
{
	std::ofstream file(fileName);


	for (int x = 0; x < gnd->width; x++)
	{
		for (int y = 0; y < gnd->height; y++)
		{
			auto cube = gnd->cubes[x][y];
			file << "v " << (x + 0) * 10 << " " << -cube->heights[2] << " " << 10 * gnd->height - (y + 1) * 10 +10<< std::endl;
			file << "v " << (x + 0) * 10 << " " << -cube->heights[0] << " " << 10 * gnd->height - (y + 0) * 10 +10<< std::endl;
			file << "v " << (x + 1) * 10 << " " << -cube->heights[1] << " " << 10 * gnd->height - (y + 0) * 10 +10<< std::endl;
			file << "v " << (x + 1) * 10 << " " << -cube->heights[3] << " " << 10 * gnd->height - (y + 1) * 10 +10<< std::endl;
		}
	}

	for (int x = 0; x < gnd->width; x++)
	{
		for (int y = 0; y < gnd->height; y++)
		{
			if (gnd->cubes[x][y]->tileUp != -1)
			{
				file << "f ";
				for (int i = 0; i < 4; i++)
					file << (x + y * gnd->width) * 4 + i + 1 << " ";
				file << std::endl;
			}
			if (gnd->cubes[x][y]->tileSide != -1)
			{
				file << "f ";
				file << (x + y * gnd->width) * 4 + 2 + 1 << " ";
				file << (x + y * gnd->width) * 4 + 3 + 1 << " ";
				file << (x + (y+1) * gnd->width) * 4 + 0 + 1 << " ";
				file << (x + (y+1) * gnd->width) * 4 + 1 + 1 << " ";
				file << std::endl;
			}

			if (gnd->cubes[x][y]->tileFront != -1)
			{
				file << "f ";
				file << (x + y * gnd->width) * 4 + 3 + 1 << " ";
				file << (x + y * gnd->width) * 4 + 0 + 1 << " ";
				file << ((x+1) + y * gnd->width) * 4 + 1 + 1 << " ";
				file << ((x+1) + y * gnd->width) * 4 + 2 + 1 << " ";
				file << std::endl;
			}
		}
	}

	int index = gnd->width * gnd->height * 4;
	int currentIndex = index + 1;

	for (auto o : rsw->objects)
	{

		if (o->type == Rsw::Object::Type::Model)
		{
			auto model = dynamic_cast<Rsw::Model*>(o);
			std::vector<int> indices;
			std::vector<glm::vec3> vertices;

			model->getWorldVerts(indices, vertices);


			for (const auto &v : vertices)
			{
				file << "v " << v.x << " " << v.y << " " << v.z << std::endl;
			}

			for (int i = 0; i < indices.size(); i+=3)
			{
				file << "f ";
				for (int ii = 0; ii < 3; ii++)
					file << (currentIndex + (i+ii)) << " ";
				file << std::endl;
			}
			currentIndex += vertices.size();
		}


	}

	
}





std::vector<glm::vec3> Map::getMapQuads()
{
	std::vector<glm::vec3> quads;

	for (int x = 0; x < gnd->width; x++)
	{
		for (int y = 0; y < gnd->height; y++)
		{
			auto cube = gnd->cubes[x][y];
			if (cube->tileUp != -1)
			{
				/*quads.push_back(glm::vec3((x + 0) * 10, -cube->heights[2], 10 * gnd->height - (y + 1) * 10 + 10));
				quads.push_back(glm::vec3((x + 0) * 10, -cube->heights[0], 10 * gnd->height - (y + 0) * 10 + 10));
				quads.push_back(glm::vec3((x + 1) * 10, -cube->heights[1], 10 * gnd->height - (y + 0) * 10 + 10));
				quads.push_back(glm::vec3((x + 1) * 10, -cube->heights[3], 10 * gnd->height - (y + 1) * 10 + 10));*/
			}
			if (cube->tileSide != -1)
			{
				quads.push_back(glm::vec3(10 * x, -cube->h3, 10 * gnd->height - 10 * y));
				quads.push_back(glm::vec3(10 * x + 10, -cube->h4, 10 * gnd->height - 10 * y));
				quads.push_back(glm::vec3(10 * x + 10, -gnd->cubes[x][y + 1]->h2, 10 * gnd->height - 10 * y));
				quads.push_back(glm::vec3(10 * x, -gnd->cubes[x][y + 1]->h1, 10 * gnd->height - 10 * y));
			}
			if (cube->tileFront != -1)
			{
				quads.push_back(glm::vec3(10 * x + 10, -cube->h2, 10 * gnd->height - 10 * y + 10));
				quads.push_back(glm::vec3(10 * x + 10, -cube->h4, 10 * gnd->height - 10 * y));
				quads.push_back(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h3, 10 * gnd->height - 10 * y));
				quads.push_back(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h1, 10 * gnd->height - 10 * y + 10));
			}
		}
	}

		/*for (int x = 0; x < gnd->width; x++)
		{
			for (int y = 0; y < gnd->height; y++)
			{
				if (gnd->cubes[x][y]->tileUp != -1)
				{
					file << "f ";
					for (int i = 0; i < 4; i++)
						file << (x + y * gnd->width) * 4 + i + 1 << " ";
					file << std::endl;
				}
				if (gnd->cubes[x][y]->tileSide != -1)
				{
					file << "f ";
					file << (x + y * gnd->width) * 4 + 2 + 1 << " ";
					file << (x + y * gnd->width) * 4 + 3 + 1 << " ";
					file << (x + (y + 1) * gnd->width) * 4 + 0 + 1 << " ";
					file << (x + (y + 1) * gnd->width) * 4 + 1 + 1 << " ";
					file << std::endl;
				}

				if (gnd->cubes[x][y]->tileFront != -1)
				{
					file << "f ";
					file << (x + y * gnd->width) * 4 + 3 + 1 << " ";
					file << (x + y * gnd->width) * 4 + 0 + 1 << " ";
					file << ((x + 1) + y * gnd->width) * 4 + 1 + 1 << " ";
					file << ((x + 1) + y * gnd->width) * 4 + 2 + 1 << " ";
					file << std::endl;
				}
			}
		}*/
	return quads;
}

glm::vec4 Map::getHeightsAt(int x, int y)
{
	assert(inMap(x, y));
	float ret[4];

	for (int i = 0; i < 4; i++)
	{
		int xx = i % 2;
		int yy = i / 2;
		if (inMap(x - xx, y - yy))
			ret[i] = gnd->cubes[x - xx][y - yy]->heights[i];
		else
			ret[i] = gnd->cubes[x][y]->heights[0];
	}
	return glm::vec4(ret[0], ret[1], ret[2], ret[3]);
}


float Map::getHeightAt(float x, float y)
{
	int ix = (int)x;
	int iy = (int)y;

	if (!inMap(ix, iy))
		return 0;

	//TODO: use barycentric coordinats
	Gnd::Cube* c = gnd->cubes[ix][iy];
	return (c->h1 + c->h2 + c->h3 + c->h4) / 4.0f;
}

void Map::exportColors(const std::string& fileName)
{
	char* data = new char[gnd->width * gnd->height * 3];
	for (int y = 0; y < gnd->height; y++)
	{
		for (int x = 0; x < gnd->width; x++)
		{
			data[(x + y * gnd->width) * 3 + 0] = 255;
			data[(x + y * gnd->width) * 3 + 1] = 255;
			data[(x + y * gnd->width) * 3 + 2] = 255;

			if (gnd->cubes[x][y]->tileUp != -1)
			{
				auto tile = gnd->tiles[gnd->cubes[x][y]->tileUp];
				data[(x + y * gnd->width) * 3 + 0] = tile->color.r;
				data[(x + y * gnd->width) * 3 + 1] = tile->color.g;
				data[(x + y * gnd->width) * 3 + 2] = tile->color.b;
			}
				
		}
	}

	stbi_write_png(fileName.c_str(), gnd->width, gnd->height, 3, data, 0);
	delete[] data;
}

void Map::importColors(const std::string& fileName)
{
	int width, height, comp;

	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &comp, 3);
	if (!data || width != gnd->width || height != gnd->height)
	{
		if (data)
			stbi_image_free(data);
		return;
	}
	this->gnd->makeTilesUnique();
	for (int y = 0; y < gnd->height; y++)
	{
		for (int x = 0; x < gnd->width; x++)
		{
			if (gnd->cubes[x][y]->tileUp != -1)
			{
				auto tile = gnd->tiles[gnd->cubes[x][y]->tileUp];
				tile->color.r = data[(x + y * gnd->width) * 3 + 0];
				tile->color.g = data[(x + y * gnd->width) * 3 + 1];
				tile->color.b = data[(x + y * gnd->width) * 3 + 2];
			}

		}
	}
}




template class blib::BackgroundTask<int>;

