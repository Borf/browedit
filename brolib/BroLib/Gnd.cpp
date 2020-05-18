#include "Gnd.h"

#include <set>

#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>
#include <blib/linq.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
using blib::util::Log;

Gnd::Gnd(int width, int height)
{
	version = 0x0107;
	this->width = width;
	this->height = height;
	tileScale = 10.0f;
	maxTexName = 80;
	lightmapWidth = 8;
	lightmapHeight = 8;
	gridSizeCell = 1;


	cubes.resize(width, std::vector<Cube*>(height, NULL));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Cube* cube = new Cube();
			cube->h1 = cube->h2 = cube->h3 = cube->h4 = 0;
			cube->tileUp = -1;
			cube->tileSide = -1;
			cube->tileFront = -1;
			cube->calcNormal();
			cubes[x][y] = cube;
		}
	}
}



Gnd::Gnd( const std::string &fileName )
{
	blib::util::StreamInFile* file = blib::util::FileSystem::openRead(fileName + ".gnd");
	if(!file || !file->opened())
	{
		width = 0;
		height = 0;
		Log::err<<"GND: Unable to open gnd file: "<<fileName<<".gnd"<<Log::newline;
		return;
	}
	Log::out<<"GND: Reading gnd file"<<Log::newline;
	char header[4];
	file->read(header, 4);
	if(header[0] == 'G' && header[1] == 'R' && header[2] == 'G' && header[3] == 'N')
		version = file->readShort();
	else
	{
		version = 0;
		Log::out << "GND: Invalid GND file" << Log::newline;
	}

	int textureCount = 0;

	if(version > 0)
	{
		width = file->readInt();
		height = file->readInt();
		tileScale = file->readFloat();
		textureCount = file->readInt();
		maxTexName = file->readInt();// 80
	}
	else
	{
		file->seek(6, blib::util::StreamSeekable::CURRENT);//TODO: test this
		width = file->readInt();
		height = file->readInt();
		textureCount = file->readInt();
	}


	textures.reserve(textureCount);
	for(int i = 0; i < textureCount; i++)
	{
		Texture* texture = new Texture();
		texture->file = file->readString(40);
		texture->name = file->readString(40);

		//texture->texture = new Texture("data/texture/" + texture->file); //TODO: blib loader

		textures.push_back(texture);
	}


	if(version > 0)
	{
		int lightmapCount = file->readInt();
		lightmapWidth = file->readInt();
		lightmapHeight = file->readInt();
		gridSizeCell = file->readInt();

		//Fix lightmap format if it was invalid. by Henko
		if (lightmapWidth != 8 || lightmapHeight != 8 || gridSizeCell != 1)
		{
			Log::err<<"GND: Invalid Lightmap Format in "<<fileName<<".gnd"<<Log::newline;
			lightmapWidth = 8;
			lightmapHeight = 8;
			gridSizeCell = 1;
		}

		lightmaps.reserve(lightmapCount);
		for(int i = 0; i < lightmapCount; i++)
		{
			Lightmap* lightmap = new Lightmap();
			file->read((char*)lightmap->data, 256);
			lightmaps.push_back(lightmap);
		}

		int tileCount = file->readInt();
		Log::out << "Tilecount: " << tileCount << Log::newline;
		tiles.reserve(tileCount);
		for(int i = 0; i < tileCount; i++)
		{
			Tile* tile = new Tile();

			tile->v1.x = file->readFloat();
			tile->v2.x = file->readFloat();
			tile->v3.x = file->readFloat();
			tile->v4.x = file->readFloat();
			tile->v1.y = file->readFloat();
			tile->v2.y = file->readFloat();
			tile->v3.y = file->readFloat();
			tile->v4.y = file->readFloat();
			tile->textureIndex = file->readWord();
			tile->lightmapIndex= file->readUWord();


			if (tile->lightmapIndex < 0 || tile->lightmapIndex == (unsigned short)-1)
			{
				Log::out << "Lightmapindex < 0" << Log::newline;
				tile->lightmapIndex = 0;
			}



			tile->color.b = (unsigned char)file->get();
			tile->color.g = (unsigned char)file->get();
			tile->color.r = (unsigned char)file->get();
			tile->color.a = (unsigned char)file->get();
			tiles.push_back(tile);
		}




		cubes.resize(width, std::vector<Cube*>(height, NULL));
		for(int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Cube* cube = new Cube();
				cube->h1 = file->readFloat();
				cube->h2 = file->readFloat();
				cube->h3 = file->readFloat();
				cube->h4 = file->readFloat();
				cube->calcNormal();

				if (version >= 0x0106)
				{
					cube->tileUp = file->readInt();
					cube->tileSide = file->readInt();
					cube->tileFront = file->readInt();
				}
				else
				{
					cube->tileUp = file->readUWord();
					cube->tileSide = file->readUWord();
					cube->tileFront = file->readUWord();
				}

				if (cube->tileUp >= (int)tiles.size())
				{
					Log::out << "Wrong value for tileup at " << x << ", " << y << "Found "<<cube->tileUp<<", but only "<<tiles.size()<<" tiles found"<<Log::newline;
					cube->tileUp = -1;
				}
				if (cube->tileSide >= (int)tiles.size())
				{
					Log::out << "Wrong value for tileside at " << x << ", " << y << Log::newline;
					cube->tileSide = -1;
				}
				if (cube->tileFront >= (int)tiles.size())
				{
					Log::out << "Wrong value for tilefront at " << x << ", " << y << Log::newline;
					cube->tileFront = -1;
				}

				cubes[x][y] = cube;
			}
		}
	}
	else
	{
		//TODO: port code...too lazy for now
	}
	delete file;
	Log::out<<"GND: Done reading gnd file"<<Log::newline;

	for (int x = 1; x < width-1; x++)
	{
		for (int y = 1; y < height-1; y++)
		{
			cubes[x][y]->calcNormals(this, x, y);
		}
	}
	Log::out<<"GND: Done calculating normals" << Log::newline;
}

Gnd::~Gnd()
{
	blib::linq::deleteall(textures);
	blib::linq::deleteall(lightmaps);
	blib::linq::deleteall(tiles);

	for (size_t i = 0; i < cubes.size(); i++)
		for (size_t ii = 0; ii < cubes[i].size(); ii++)
			if (cubes[i][ii])
				delete cubes[i][ii];

}




void Gnd::save(std::string fileName)
{
	cleanTiles();
	blib::util::PhysicalFileSystemHandler::StreamOutFilePhysical* pFile = new blib::util::PhysicalFileSystemHandler::StreamOutFilePhysical(fileName + ".gnd");

	char header[5] = "GRGN";
	pFile->write(header, 4);
	pFile->writeShort(version);

	if (version > 0)
	{
		pFile->writeInt(width);
		pFile->writeInt(height);
		pFile->writeFloat(tileScale);
		pFile->writeInt(textures.size());
		pFile->writeInt(maxTexName);// iunno
	}
	
	
	for (size_t i = 0; i < textures.size(); i++)
	{
		pFile->writeString(textures[i]->file, 40);
		pFile->writeString(textures[i]->name, 40);
	}


	if (version > 0)
	{
		pFile->writeInt(lightmaps.size());
		pFile->writeInt(lightmapWidth);
		pFile->writeInt(lightmapHeight);
		pFile->writeInt(gridSizeCell);

		for (size_t i = 0; i < lightmaps.size(); i++)
			pFile->write((char*)lightmaps[i]->data, 256);

		pFile->writeInt(tiles.size());
		for (size_t i = 0; i < tiles.size(); i++)
		{
			Tile* tile = tiles[i];

			pFile->writeFloat(tile->v1.x);
			pFile->writeFloat(tile->v2.x);
			pFile->writeFloat(tile->v3.x);
			pFile->writeFloat(tile->v4.x);
			pFile->writeFloat(tile->v1.y);
			pFile->writeFloat(tile->v2.y);
			pFile->writeFloat(tile->v3.y);
			pFile->writeFloat(tile->v4.y);
			pFile->writeUWord(tile->textureIndex);
			pFile->writeUWord(tile->lightmapIndex);
	
			pFile->put(tile->color.b);
			pFile->put(tile->color.g);
			pFile->put(tile->color.r);
			pFile->put(tile->color.a);
		}

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Cube* cube = cubes[x][y];
				pFile->writeFloat(cube->h1);
				pFile->writeFloat(cube->h2);
				pFile->writeFloat(cube->h3);
				pFile->writeFloat(cube->h4);
				if (version >= 0x0106)
				{
					pFile->writeInt(cube->tileUp);
					pFile->writeInt(cube->tileSide);
					pFile->writeInt(cube->tileFront);
				}
				else
				{
					pFile->writeUWord(cube->tileUp);
					pFile->writeUWord(cube->tileSide);
					pFile->writeUWord(cube->tileFront);
				}
			}
		}


	}


	delete pFile;
}


Gnd::Texture::Texture()
{
	texture = NULL;
}

Gnd::Texture::~Texture()
{
	if(texture)
		blib::ResourceManager::getInstance().dispose(texture);
	texture = NULL;
}

void Gnd::Cube::calcNormal()
{
/*
	1----2
	|\   |
	| \  |
	|  \ |
	|   \|
	3----4
*/
	glm::vec3 v1(10, -h1, 0);
	glm::vec3 v2(0, -h2, 0);
	glm::vec3 v3(10, -h3, 10);
	glm::vec3 v4(0, -h4, 10);

	glm::vec3 normal1 = glm::normalize(glm::cross(v4 - v3, v1 - v3));
	glm::vec3 normal2 = glm::normalize(glm::cross(v1 - v2, v4 - v2));
	normal = glm::normalize(normal1 + normal2);
	for (int i = 0; i < 4; i++)
		normals[i] = normal;
}


void Gnd::Cube::calcNormals(Gnd* gnd, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		normals[i] = glm::vec3(0, 0, 0);
		for (int ii = 0; ii < 4; ii++)
		{
			int xx = (ii % 2) * ((i % 2 == 0) ? -1 : 1);
			int yy = (ii / 2) * (i < 2 ? -1 : 1);
			if (x+xx >= 0 && x+xx < gnd->width && y+yy >= 0 && y+yy < gnd->height)
				normals[i] += gnd->cubes[x + xx][y + yy]->normal;
		}
		normals[i] = glm::normalize(normals[i]);
	}
}


void Gnd::makeLightmapsUnique()
{
	makeTilesUnique();
	std::set<int> taken;
	for (Tile* t : tiles)
	{
		if (taken.find(t->lightmapIndex) == taken.end())
			taken.insert(t->lightmapIndex);
		else
		{
			Lightmap* l = new Lightmap(*lightmaps[t->lightmapIndex]);
			t->lightmapIndex = lightmaps.size();
			lightmaps.push_back(l);
		}
	}
}

void Gnd::makeLightmapsClear()
{
	lightmaps.clear();
	Lightmap* l = new Lightmap();
	memset(l->data, 255, 64);
	memset(l->data+64, 0, 256-64);
	lightmaps.push_back(l);

	for (Tile* t : tiles)
		t->lightmapIndex = 0;
}
void Gnd::makeLightmapBorders()
{
	makeLightmapsUnique();
	Log::out << "Fixing borders" << Log::newline;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Gnd::Cube* cube = cubes[x][y];
			int tileId = cube->tileUp;
			if (tileId != -1)
			{
				Gnd::Tile* tile = tiles[tileId];
				assert(tile && tile->lightmapIndex != -1);
				Gnd::Lightmap* lightmap = lightmaps[tile->lightmapIndex];

				for (int i = 0; i < 8; i++)
				{
					lightmap->data[i + 8 * 0] = getLightmapBrightness(x, y - 1, i, 6);
					lightmap->data[i + 8 * 7] = getLightmapBrightness(x, y + 1, i, 1);
					lightmap->data[0 + 8 * i] = getLightmapBrightness(x - 1, y, 6, i);
					lightmap->data[7 + 8 * i] = getLightmapBrightness(x + 1, y, 1, i);

					for (int c = 0; c < 3; c++)
					{
						lightmap->data[64 + 3 * (i + 8 * 0) + c] = getLightmapColor(x, y - 1, i, 6)[c];
						lightmap->data[64 + 3 * (i + 8 * 7) + c] = getLightmapColor(x, y + 1, i, 1)[c];
						lightmap->data[64 + 3 * (0 + 8 * i) + c] = getLightmapColor(x - 1, y, 6, i)[c];
						lightmap->data[64 + 3 * (7 + 8 * i) + c] = getLightmapColor(x + 1, y, 1, i)[c];

					}

				}
			}
			tileId = cube->tileSide;
			if (tileId != -1)
			{
				Gnd::Tile* tile = tiles[tileId];
				assert(tile && tile->lightmapIndex != -1);
				Gnd::Lightmap* lightmap = lightmaps[tile->lightmapIndex];

				auto otherCube = cubes[x-1][y];
				if (otherCube->tileSide != -1)
				{
					auto otherTile = tiles[otherCube->tileSide];
					auto otherLightmap = lightmaps[otherTile->lightmapIndex];

					for (int i = 0; i < 8; i++)
						lightmap->data[0 + 8 * i] = otherLightmap->data[6 + 8 * i];
				}
				else
					for (int i = 0; i < 8; i++)
						lightmap->data[0 + 8 * i] = lightmap->data[1 + 8 * i];

				otherCube = cubes[x+1][y];
				if (otherCube->tileSide != -1)
				{
					auto otherTile = tiles[otherCube->tileSide];
					auto otherLightmap = lightmaps[otherTile->lightmapIndex];

					for (int i = 0; i < 8; i++)
						lightmap->data[7 + 8 * i] = otherLightmap->data[1 + 8 * i];
				}
				else
					for (int i = 0; i < 8; i++)
						lightmap->data[7 + 8 * i] = lightmap->data[6 + 8 * i];

				//top and bottom
				otherCube = cubes[x][y+1];
				if (otherCube->tileUp != -1)
				{
					auto otherTile = tiles[otherCube->tileUp];
					auto otherLightmap = lightmaps[otherTile->lightmapIndex];

					for (int i = 0; i < 8; i++)
						lightmap->data[i + 8 * 7] = otherLightmap->data[i + 8 * 1]; //bottom
				}
				else
					for (int i = 0; i < 8; i++)
						lightmap->data[i + 8 * 7] = lightmap->data[i + 8 * 6];


				otherCube = cubes[x][y];
				if (otherCube->tileUp != -1)
				{
					auto otherTile = tiles[otherCube->tileUp];
					auto otherLightmap = lightmaps[otherTile->lightmapIndex];

					for (int i = 0; i < 8; i++)
						lightmap->data[i + 8 * 0] = otherLightmap->data[i + 8 * 1]; //bottom
				}
				else
					for (int i = 0; i < 8; i++)
						lightmap->data[i + 8 * 0] = lightmap->data[i + 8 * 1];
			}

			tileId = cube->tileFront;
			if (tileId != -1)
			{
				Gnd::Tile* tile = tiles[tileId];
				assert(tile && tile->lightmapIndex != -1);
				Gnd::Lightmap* lightmap = lightmaps[tile->lightmapIndex];

				if (y < width)
				{
					auto otherCube = cubes[x][y + 1];
					if (otherCube->tileFront != -1)
					{
						auto otherTile = tiles[otherCube->tileFront];
						auto otherLightmap = lightmaps[otherTile->lightmapIndex];

						for (int i = 0; i < 8; i++)
							lightmap->data[0 + 8 * i] = otherLightmap->data[6 + 8 * i];
					}
					else
						for (int i = 0; i < 8; i++)
							lightmap->data[0 + 8 * i] = lightmap->data[1 + 8 * i];
				}

				if (y > 0)
				{
					auto otherCube = cubes[x][y - 1];
					if (otherCube->tileFront != -1)
					{
						auto otherTile = tiles[otherCube->tileFront];
						auto otherLightmap = lightmaps[otherTile->lightmapIndex];

						for (int i = 0; i < 8; i++)
							lightmap->data[7 + 8 * i] = otherLightmap->data[1 + 8 * i];
					}
					else
						for (int i = 0; i < 8; i++)
							lightmap->data[7 + 8 * i] = lightmap->data[6 + 8 * i];
				}

				if (x < width)
				{
					//top and bottom
					auto otherCube = cubes[x + 1][y];
					if (otherCube->tileUp != -1)
					{
						auto otherTile = tiles[otherCube->tileUp];
						auto otherLightmap = lightmaps[otherTile->lightmapIndex];

						for (int i = 0; i < 8; i++)
							lightmap->data[i + 8 * 7] = otherLightmap->data[i + 8 * 1]; //bottom
					}
					else
						for (int i = 0; i < 8; i++)
							lightmap->data[i + 8 * 7] = lightmap->data[i + 8 * 6];
				}

				auto otherCube = cubes[x][y];
				if (otherCube->tileUp != -1)
				{
					auto otherTile = tiles[otherCube->tileUp];
					auto otherLightmap = lightmaps[otherTile->lightmapIndex];

					for (int i = 0; i < 8; i++)
						lightmap->data[i + 8 * 0] = otherLightmap->data[i + 8 * 1]; //bottom
				}
				else
					for (int i = 0; i < 8; i++)
						lightmap->data[i + 8 * 0] = lightmap->data[i + 8 * 1];
			}

		}
	}


}


void Gnd::makeLightmapBorders(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
	Gnd::Cube* cube = cubes[x][y];
	int tileId = cube->tileUp;
	if (tileId == -1)
		return;
	Gnd::Tile* tile = tiles[tileId];
	assert(tile && tile->lightmapIndex != -1);
	Gnd::Lightmap* lightmap = lightmaps[tile->lightmapIndex];

	for (int i = 0; i < 8; i++)
	{
		lightmap->data[i + 8 * 0] = getLightmapBrightness(x, y - 1, i, 6);
		lightmap->data[i + 8 * 7] = getLightmapBrightness(x, y + 1, i, 1);
		lightmap->data[0 + 8 * i] = getLightmapBrightness(x - 1, y, 6, i);
		lightmap->data[7 + 8 * i] = getLightmapBrightness(x + 1, y, 1, i);

		for (int c = 0; c < 3; c++)
		{
			lightmap->data[64 + 3 * (i + 8 * 0) + c] = getLightmapColor(x, y - 1, i, 6)[c];
			lightmap->data[64 + 3 * (i + 8 * 7) + c] = getLightmapColor(x, y + 1, i, 1)[c];
			lightmap->data[64 + 3 * (0 + 8 * i) + c] = getLightmapColor(x - 1, y, 6, i)[c];
			lightmap->data[64 + 3 * (7 + 8 * i) + c] = getLightmapColor(x + 1, y, 1, i)[c];

		}
	}
}


int Gnd::getLightmapBrightness(int x, int y, int lightmapX, int lightmapY)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
		return 0;

	Gnd::Cube* cube = cubes[x][y];
	int tileId = cube->tileUp;
	if (tileId == -1)
		return 0;
	Gnd::Tile* tile = tiles[tileId];
	assert(tile && tile->lightmapIndex != -1);
	Gnd::Lightmap* lightmap = lightmaps[tile->lightmapIndex];

	return lightmap->data[lightmapX + 8 * lightmapY];

}

glm::ivec3 Gnd::getLightmapColor(int x, int y, int lightmapX, int lightmapY)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
		return glm::ivec3(0, 0, 0);

	Gnd::Cube* cube = cubes[x][y];
	int tileId = cube->tileUp;
	if (tileId == -1)
		return glm::ivec3(0,0,0);
	Gnd::Tile* tile = tiles[tileId];
	assert(tile && tile->lightmapIndex != -1);
	Gnd::Lightmap* lightmap = lightmaps[tile->lightmapIndex];

	return glm::ivec3(
		lightmap->data[64 + (lightmapX + 8 * lightmapY) * 3 + 0],
		lightmap->data[64 + (lightmapX + 8 * lightmapY) * 3 + 1],
		lightmap->data[64 + (lightmapX + 8 * lightmapY) * 3 + 2]);


}

void Gnd::cleanLightmaps()
{
	Log::out << "Lightmap cleanup, starting with " << lightmaps.size() << " lightmaps" << Log::newline;
	std::map<unsigned char, std::vector<std::size_t>> lookup;

	for(int i = 0; i < (int)lightmaps.size(); i++)
	{
		unsigned char hash = lightmaps[i]->hash();
		bool found = false;
		if (lookup.find(hash) != lookup.end())
		{
			for (const auto ii : lookup[hash])
			{
				if ((*lightmaps[i]) == (*lightmaps[ii]))
				{// if it is found
					assert(i > ii);
					//change all tiles with lightmap i to ii
					for (auto tile : tiles)
						if (tile->lightmapIndex == i)
							tile->lightmapIndex = ii;
						else if (tile->lightmapIndex > i)
							tile->lightmapIndex--;
					//remove lightmap i
					delete lightmaps[i];
					lightmaps.erase(lightmaps.begin() + i);
					i--;
					found = true;
					break;
				}
			}
		}
		if (!found)
		{
			lookup[hash].push_back(i);
		}

	}

	Log::out << "Lightmap cleanup, ending with " << lightmaps.size() << " lightmaps" << Log::newline;

}


void Gnd::makeLightmapsSmooth()
{
	Log::out << "Smoothing..." << Log::newline;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Gnd::Cube* cube = cubes[x][y];
			int tileId = cube->tileUp;
			if (tileId == -1)
				continue;
			Gnd::Tile* tile = tiles[tileId];
			assert(tile && tile->lightmapIndex != -1);
			Gnd::Lightmap* lightmap = lightmaps[tile->lightmapIndex];

			char newData[64];

			for (int xx = 1; xx < 7; xx++)
			{
				for (int yy = 1; yy < 7; yy++)
				{
					int total = 0;
					for (int xxx = xx - 1; xxx <= xx + 1; xxx++)
						for (int yyy = yy - 1; yyy <= yy + 1; yyy++)
							total += lightmap->data[xxx + 8 * yyy];
					newData[xx + 8 * yy] = total / 9;
				}
			}
			memcpy(lightmap->data, newData, 64 * sizeof(char));
		}
	}

}

void Gnd::cleanTiles()
{
	Log::out << "Tiles cleanup, starting with " << tiles.size() << " tiles" << Log::newline;
	std::set<int> used;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			used.insert(cubes[x][y]->tileUp);
			used.insert(cubes[x][y]->tileFront);
			used.insert(cubes[x][y]->tileSide);
		}


	std::list<int> toRemove;
	for (std::size_t i = 0; i < tiles.size(); i++)
		if (used.find(i) == used.end())
			toRemove.push_back(i);
	toRemove.reverse();

	for (int i : toRemove)
	{
		tiles.erase(tiles.begin() + i);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (cubes[x][y]->tileUp > i)
					cubes[x][y]->tileUp--;
				if (cubes[x][y]->tileSide > i)
					cubes[x][y]->tileSide--;
				if (cubes[x][y]->tileFront > i)
					cubes[x][y]->tileFront--;
			}
		}
	}
	Log::out << "Tiles cleanup, ending with " << tiles.size() << " tiles" << Log::newline;
}


void Gnd::makeTilesUnique()
{
	std::set<int> taken;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (cubes[x][y]->tileUp == -1)
				continue;
			if (taken.find(cubes[x][y]->tileUp) == taken.end())
				taken.insert(cubes[x][y]->tileUp);
			else
			{
				Tile* t = new Tile(*tiles[cubes[x][y]->tileUp]);
				cubes[x][y]->tileUp = tiles.size();
				tiles.push_back(t);
			}
		}
	}
}

const unsigned char Gnd::Lightmap::hash() const //actually a crc, but will work
{
#define POLY 0x82f63b78
	unsigned char crc = ~0;
	for(int i = 0; i < 256; i++) {
		crc ^= data[i];
		for (int k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
	}
	return ~crc;
}

bool Gnd::Lightmap::operator==(const Lightmap& other) const
{
	return memcmp(data, other.data, 256) == 0;
}

const unsigned char Gnd::Tile::hash() const
{
#define POLY 0x82f63b78
	unsigned char crc = ~0;
	for (int i = 0; i < sizeof(Tile); i++) {
		crc ^= ((char*)this)[i];
		for (int k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
	}
	return ~crc;
}

bool Gnd::Tile::operator==(const Gnd::Tile& o) const
{
	return	v1 == o.v1 &&
			v2 == o.v2 &&
			v3 == o.v3 &&
			v4 == o.v4 &&
			textureIndex == o.textureIndex &&
			lightmapIndex == o.lightmapIndex &&
			color == o.color;
}
