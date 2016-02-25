#include "Gat.h"

#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>

using blib::util::Log;


Gat::Gat(const std::string &fileName)
{
	blib::util::StreamInFile* file = blib::util::FileSystem::openRead(fileName + ".gat");
	if (!file)
	{
		height = 0;
		width = 0;
		Log::err << "GND: Unable to open gat file: " << fileName << ".gat" << Log::newline;
		return;
	}
	Log::out << "GND: Reading gat file" << Log::newline;


	char header[4];
	file->read(header, 4);
	if (header[0] == 'G' && header[1] == 'R' && header[2] == 'A' && header[3] == 'T')
		version = file->readShort();
	else
		version = 0;

	width = file->readInt();
	height = file->readInt();


	tiles.resize(width, std::vector<Tile*>(height, NULL));

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Tile* tile = new Tile();
			for (int i = 0; i < 4; i++)
				tile->heights[i] = file->readFloat();
			tile->type = file->readInt();
			tiles[x][y] = tile;
		}
	}
}

Gat::Gat(int width, int height)
{
	this->width = width;
	this->height = height;
	this->version = 0x0102;
	tiles.resize(width, std::vector<Tile*>(height, NULL));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Tile* tile = new Tile();
			for (int i = 0; i < 4; i++)
				tile->heights[i] = 0;
			tile->type = 0;
			tiles[x][y] = tile;
		}
	}
}

Gat::~Gat()
{
	for (size_t i = 0; i < tiles.size(); i++)
		for (size_t ii = 0; ii < tiles[i].size(); ii++)
			if (tiles[i][ii])
				delete tiles[i][ii];
	tiles.clear();
}

void Gat::save(const std::string &fileName)
{
	blib::util::PhysicalFileSystemHandler::StreamOutFilePhysical* pFile = new blib::util::PhysicalFileSystemHandler::StreamOutFilePhysical(fileName + ".gat");


	char header[5] = "GRAT";
	pFile->write(header, 4);
	pFile->writeShort(version);

	pFile->writeInt(width);
	pFile->writeInt(height);


	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Tile* tile = tiles[x][y];
			for (int i = 0; i < 4; i++)
				pFile->writeFloat(tile->heights[i]);
			pFile->writeInt(tile->type);
		}
	}


	delete pFile;
}




