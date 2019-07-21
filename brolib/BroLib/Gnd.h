#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace blib { class Texture; }

class Gnd
{
public:
	Gnd(const std::string &fileName);
	Gnd(int width, int height);
	~Gnd();

	void save(std::string fileName);

	class Texture
	{
	public:
		std::string name;
		std::string file;

		blib::Texture* texture;
		Texture();
		~Texture();
	};

	class Lightmap
	{
	public:
		Lightmap() { memset(data, 0, 256 * sizeof(unsigned char)); };
		Lightmap(const Lightmap& other)
		{
			memcpy(data, other.data, 256 * sizeof(unsigned char));
		}
		unsigned char data[256];
		const unsigned char hash() const;
		bool operator == (const Lightmap& other) const;
	};

	class Tile
	{
	public:
		Tile() : textureIndex(-1), lightmapIndex(-1) {};
		Tile(const Tile& o) {
			v1 = o.v1;
			v2 = o.v2;
			v3 = o.v3;
			v4 = o.v4;
			textureIndex = o.textureIndex;
			lightmapIndex = o.lightmapIndex;
			color = o.color;
		}
		glm::vec2 v1,v2,v3,v4;
		int textureIndex;
		int lightmapIndex;
		glm::ivec4 color;
		const unsigned char hash() const;
		bool operator == (const Tile& other) const;
	};

	class Cube
	{
	public:
		Cube() 
		{
			selected = rand()%100 == 0;
		}
		union 
		{
			struct
			{
				float h1, h2, h3, h4;
			};
			float heights[4];
		};
		union
		{
			struct
			{
				int tileUp, tileSide, tileFront;
			};
			int tileIds[3];
		};
		bool selected;

		glm::vec3 normal;
		glm::vec3 normals[4];

		void calcNormal();
		void calcNormals(Gnd* gnd, int x, int y);
	};


	short version;
	int width;
	int height;
	float tileScale;
	int maxTexName;
	int lightmapWidth;
	int lightmapHeight;
	int gridSizeCell;
	std::vector<Texture*> textures;
	std::vector<Lightmap*> lightmaps;
	std::vector<Tile*> tiles;
	std::vector<std::vector<Cube*> > cubes;


	void makeLightmapsUnique();
	void makeLightmapsClear();
	void makeLightmapBorders();
	void makeLightmapBorders(int x, int y);
	int getLightmapBrightness(int x, int y, int lightmapX, int lightmapY);
	glm::ivec3 getLightmapColor(int x, int y, int lightmapX, int lightmapY);
	void makeLightmapsSmooth();
	void makeTilesUnique();
	void cleanLightmaps();
	void cleanTiles();

};