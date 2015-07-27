#include "Map.h"
#include "Gnd.h"
#include "Rsw.h"
#include "Gat.h"

#include <blib/BackgroundTask.h>
#include <blib/util/Log.h>
using blib::util::Log;


#include <blib/util/stb_image_create.h>
#include <blib/util/stb_image.h>


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


template class blib::BackgroundTask<int>;

