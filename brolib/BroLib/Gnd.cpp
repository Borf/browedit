#include "Gnd.h"

#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>
#include <blib/Texture.h>
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
			cubes[x][y] = cube;
		}
	}
}



Gnd::Gnd( const std::string &fileName )
{
	blib::util::StreamInFile* file = blib::util::FileSystem::openRead(fileName + ".gnd");
	if(!file)
	{
		Log::err<<"GND: Unable to open gnd file: "<<fileName<<".gnd"<<Log::newline;
		return;
	}
	Log::out<<"GND: Reading gnd file"<<Log::newline;

	if(!file)
	{
		width = 0;
		height = 0;
		Log::err<<"GND: Error opening map "<<fileName<<"..."<<Log::newline;
		return;
	}
	char header[4];
	file->read(header, 4);
	if(header[0] == 'G' && header[1] == 'R' && header[2] == 'G' && header[3] == 'N')
		version = file->readShort();
	else
		version = 0;

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
			file->read(lightmap->data, 256);
			lightmaps.push_back(lightmap);
		}

		int tileCount = file->readInt();
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
			tile->lightmapIndex= file->readWord();

			if (tile->lightmapIndex < 0)
			{
				tile->lightmapIndex = 0;
			}

			tile->color = glm::vec4((unsigned char)file->get() / 255.0f, (unsigned char)file->get() / 255.0f, (unsigned char)file->get() / 255.0f, (unsigned char)file->get() / 255.0f);
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
				if (version >= 0x0106)
				{
					cube->tileUp = file->readInt();
					cube->tileSide = file->readInt();
					cube->tileFront = file->readInt();
				}
				else
				{
					cube->tileUp = file->readWord();
					cube->tileSide = file->readWord();
					cube->tileFront = file->readWord();
				}

				if (cube->tileUp >= (int)tiles.size())
				{
					Log::out << "Wrong value for tileup at " << x << ", " << y << Log::newline;
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
}




void Gnd::save(std::string fileName)
{
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
			pFile->write(lightmaps[i]->data, 256);

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
			pFile->writeWord(tile->textureIndex);
			pFile->writeWord(tile->lightmapIndex);
			
			pFile->put((int)(tile->color.a * 255));
			pFile->put((int)(tile->color.z * 255));
			pFile->put((int)(tile->color.y * 255));
			pFile->put((int)(tile->color.x * 255));
		}

		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
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
					pFile->writeWord(cube->tileUp);
					pFile->writeWord(cube->tileSide);
					pFile->writeWord(cube->tileFront);
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
		texture->unload();
	texture = NULL;
}
