#include "Gnd.h"

#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>
#include <blib/Texture.h>
using blib::util::Log;

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


	if(version > 0)
	{
		width = file->readInt();
		height = file->readInt();
		tileScale = file->readFloat();
		textureCount = file->readInt();
		file->readInt();// iunno
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
			tile->color = glm::vec4((unsigned char)file->get() / 256.0f, (unsigned char)file->get() / 255.0f, (unsigned char)file->get() / 255.0f, (unsigned char)file->get() / 255.0f);
			tiles.push_back(tile);
		}




		cubes.resize(width, std::vector<Cube*>(height, NULL));
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				Cube* cube = new Cube();
				cube->h1 = file->readFloat();
				cube->h2 = file->readFloat();
				cube->h3 = file->readFloat();
				cube->h4 = file->readFloat();
				if(version >= 0x0106)
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


Gnd::Texture::Texture()
{
	texture = NULL;
}

Gnd::Texture::~Texture()
{
	if(texture)
		delete texture;
	texture = NULL;
}
