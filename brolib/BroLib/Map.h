#pragma once


#include <vector>
#include <string>
class Gnd;
class Rsw;
class Gat;

#include "Gnd.h"

class Map
{
	Gnd* gnd;
	Rsw* rsw;
	Gat* gat;

	std::string fileName;
public:
	Map(const std::string &fileName);
	Map(const std::string &fileName, int width, int height);
	~Map();

	inline Gnd* getGnd() const
	{
		return gnd;
	}
	inline Rsw* getRsw() const
	{
		return rsw;
	}
	inline Gat* getGat() const
	{
		return gat;
	}

	inline const std::string getFileName() const
	{
		return fileName;
	}

	void save(const std::string &filename);
	bool inMap(int xx, int yy);
	void saveHeightmap(const std::string &fileName);
	void loadHeightmap(const std::string &fileName);
	void exportObj(const std::string &fileName);

	float heightImportMin;
	float heightImportMax;
	std::vector<std::vector<Gnd::Cube> > heightImportCubes;


	glm::vec4 getHeightsAt(int x, int y);
	float getHeightAt(float x, float y);
};