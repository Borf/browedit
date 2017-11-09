#include "BrowEdit.h"

#include "windows/TextureWindow.h"
#include "windows/ObjectWindow.h"

#include <blib/util/Log.h>
using blib::util::Log;

#include <BroLib/Map.h>

#include <thread>
#include <atomic>

void BrowEdit::menuActionsLightmapCalculate()
{
	if (!map)
		return;
	Log::out << "Making lightmaps unique" << Log::newline;
	map->getGnd()->makeLightmapsUnique();
	mapRenderer.setAllDirty();

	int height = map->getGnd()->height;
	const float s = 10 / 6.0f;
	Log::out << "Making lightmap..." << Log::newline;



	std::vector<Rsw::Light*> lights;
	for (auto &o : map->getRsw()->objects)
		if (o->type == Rsw::Object::Type::Light)
			lights.push_back(dynamic_cast<Rsw::Light*>(o));


	glm::vec3 lightDirection;
	lightDirection[0] = -glm::cos(glm::radians((float)map->getRsw()->light.longitude)) * glm::sin(glm::radians((float)map->getRsw()->light.latitude));
	lightDirection[1] = glm::cos(glm::radians((float)map->getRsw()->light.latitude));
	lightDirection[2] = glm::sin(glm::radians((float)map->getRsw()->light.longitude)) * glm::sin(glm::radians((float)map->getRsw()->light.latitude));


	const float quality = 1.0f / 4.0f;


	auto calculateLight = [this, lightDirection, &lights](const glm::vec3 &groundPos, const glm::vec3 &normal)
	{
		int intensity = 0;

		if (map->getRsw()->light.lightmapAmbient > 0)
			intensity = (int)(map->getRsw()->light.lightmapAmbient * 255);


		if (map->getRsw()->light.lightmapIntensity > 0 && glm::dot(normal, lightDirection) > 0)
		{
			blib::math::Ray ray(groundPos, glm::normalize(lightDirection));
			bool collides = false;
			for (Rsw::Object* o : map->getRsw()->objects)
			{
				auto model = dynamic_cast<Rsw::Model*>(o);
				if (model)
					if (model->collidesTexture(ray))
					{
						collides = true;
						break;
					}
			}
			if (!collides)
			{
				intensity += (int)(map->getRsw()->light.lightmapIntensity * 255);
			}
		}

		for (auto light : lights)
		{
			glm::vec3 lightPosition(5 * map->getGnd()->width + light->position.x, -light->position.y, 5 * map->getGnd()->height - light->position.z);

			float distance = glm::distance(lightPosition, groundPos);
			if (distance > light->realRange())
				continue;

			float d = glm::max(distance - light->range, 0.0f);
			float denom = d / light->range + 1;
			float attenuation = light->intensity / (denom * denom);
			if (light->cutOff > 0)
				attenuation = glm::max(0.0f, (attenuation - light->cutOff) / (1 - light->cutOff));


			blib::math::Ray ray(groundPos, glm::normalize(lightPosition - groundPos));
			bool collides = false;
			for (Rsw::Object* o : map->getRsw()->objects)
			{
				if (o->collides(ray))
				{
					collides = true;
					break;
				}
			}
			if (!collides)
			{
				intensity += (int)attenuation;
			}
		}

		return intensity;
	};


	auto calcPos = [this, quality, s, height, &calculateLight](int direction, int tileId, int x, int y)
	{
		Gnd::Tile* tile = map->getGnd()->tiles[tileId];
		assert(tile && tile->lightmapIndex != -1);
		Gnd::Lightmap* lightmap = map->getGnd()->lightmaps[tile->lightmapIndex];

		Gnd::Cube* cube = map->getGnd()->cubes[x][y];

		for (int xx = 1; xx < 7; xx++)
		{
			for (int yy = 1; yy < 7; yy++)
			{
				int totalIntensity = 0;
				int count = 0;
				for (float xxx = 0; xxx < 1; xxx += quality)
				{
					for (float yyy = 0; yyy < 1; yyy += quality)
					{
						glm::vec3 groundPos;
						glm::vec3 normal;
						//todo: use proper height using barycentric coordinats
						if (direction == 0)
						{
							groundPos = glm::vec3(10 * x + s * ((xx + xxx) - 1), -(cube->h1 + cube->h2 + cube->h3 + cube->h4) / 4.0f, 10 * height + 10 - 10 * y - s * ((yy + yyy) - 1));
							normal = glm::vec3(0, 1, 0);
						}
						else if (direction == 1) //front
						{
							auto otherCube = map->getGnd()->cubes[x][y + 1];
							float h1 = glm::mix(cube->h3, cube->h4, ((xx + xxx) - 1) / 6.0f);
							float h2 = glm::mix(otherCube->h2, otherCube->h1, ((xx + xxx) - 1) / 6.0f);
							float h = glm::mix(h1, h2, ((yy + yyy) - 1) / 6.0f);

							groundPos = glm::vec3(10 * x + s * ((xx + xxx) - 1), -h, 10 * height - 10 * y);
							normal = glm::vec3(0, 0, 1);

							if (h1 < h2)
								normal = -normal;
						}
						else if (direction == 2) //side
						{
							auto otherCube = map->getGnd()->cubes[x + 1][y];
							float h1 = glm::mix(cube->h2, cube->h4, ((xx + xxx) - 1) / 6.0f);
							float h2 = glm::mix(otherCube->h1, otherCube->h3, ((xx + xxx) - 1) / 6.0f);
							float h = glm::mix(h1, h2, ((yy + yyy) - 1) / 6.0f);

							groundPos = glm::vec3(10 * x, -h, 10 * height + 10 - 10 * y + s * ((xx + xxx) - 1));
							normal = glm::vec3(-1, 0, 0);

							if (h1 < h2)
								normal = -normal;

						}

						totalIntensity += calculateLight(groundPos, normal);
						count++;
					}
				}

				int intensity = totalIntensity / count;
				if (intensity > 255)
					intensity = 255;

				lightmap->data[xx + 8 * yy] = intensity;
				lightmap->data[64 + 3 * (xx + 8 * yy) + 0] = 0;
				lightmap->data[64 + 3 * (xx + 8 * yy) + 1] = 0;
				lightmap->data[64 + 3 * (xx + 8 * yy) + 2] = 0;
			}
		}
	};


	std::vector<std::thread> threads;
	std::atomic<unsigned> finishedX(0);
	for (int t = 0; t < 8; t++)
	{
		threads.push_back(std::thread([&]()
		{
			for (int x; (x = finishedX++) < map->getGnd()->width;)
			{
				for (int y = 0; y < map->getGnd()->height; y++)
				{
					Gnd::Cube* cube = map->getGnd()->cubes[x][y];

					for (int i = 0; i < 3; i++)
						if (cube->tileIds[i] != -1)
							calcPos(i, cube->tileIds[i], x, y);
				}
			}
		}));
	}

	for (auto &t : threads)
		t.join();

	map->getGnd()->makeLightmapBorders();


}






void BrowEdit::menuActionsLightmapSmooth()
{
	if (!map)
		return;
	Log::out << "Making lightmaps unique" << Log::newline;
	map->getGnd()->makeLightmapsUnique();
	map->getGnd()->makeLightmapsSmooth();
	map->getGnd()->makeLightmapBorders();
	mapRenderer.setAllDirty();
}


void BrowEdit::menuActionsLightmapUnique()
{
	if (!map)
		return;
	map->getGnd()->makeLightmapsUnique();
	mapRenderer.setAllDirty();
}




void BrowEdit::menuActionsScaleDown()
{
	Map* newMap = new Map("", map->getGnd()->width / 2, map->getGnd()->height / 2);
	newMap->getRsw()->water = map->getRsw()->water;

	for (auto l : map->getGnd()->lightmaps)
		newMap->getGnd()->lightmaps.push_back(new Gnd::Lightmap(*l));

	for (auto t : map->getGnd()->textures)
	{
		auto newTexture = new Gnd::Texture();
		newTexture->file = t->file;
		newTexture->name = t->name;
		newTexture->texture = nullptr;
		newMap->getGnd()->textures.push_back(newTexture);
	}

	for (int x = 0; x < newMap->getGnd()->width; x++)
	{
		for (int y = 0; y < newMap->getGnd()->height; y++)
		{
			if (map->getGnd()->cubes[x * 2][y * 2]->tileUp != -1)
			{
				Gnd::Tile* oldTile = map->getGnd()->tiles[map->getGnd()->cubes[x * 2][y * 2]->tileUp];
				Gnd::Tile* t = new Gnd::Tile();
				t->textureIndex = oldTile->textureIndex;
				t->lightmapIndex = oldTile->lightmapIndex;
				t->color = oldTile->color;

				t->v1 = oldTile->v1;
				t->v2 = map->getGnd()->tiles[map->getGnd()->cubes[x * 2 + 1][y * 2]->tileUp]->v2;
				t->v3 = map->getGnd()->tiles[map->getGnd()->cubes[x * 2][y * 2 + 1]->tileUp]->v3;
				t->v4 = map->getGnd()->tiles[map->getGnd()->cubes[x * 2 + 1][y * 2 + 1]->tileUp]->v4;

				newMap->getGnd()->tiles.push_back(t);

				newMap->getGnd()->cubes[x][y]->tileUp = newMap->getGnd()->tiles.size() - 1;
				newMap->getGnd()->cubes[x][y]->h1 = map->getGnd()->cubes[x * 2][y * 2]->h1;
				newMap->getGnd()->cubes[x][y]->h2 = map->getGnd()->cubes[x * 2 + 1][y * 2]->h2;
				newMap->getGnd()->cubes[x][y]->h3 = map->getGnd()->cubes[x * 2][y * 2 + 1]->h3;
				newMap->getGnd()->cubes[x][y]->h4 = map->getGnd()->cubes[x * 2 + 1][y * 2 + 1]->h4;
			}

		}
	}


	for (auto o : map->getRsw()->objects)
	{

		if (o->type == Rsw::Object::Type::Model)
		{
			Rsw::Model* oldModel = dynamic_cast<Rsw::Model*>(o);

			Rsw::Model* model = new Rsw::Model();
			model->matrixCached = false;
			if (version >= 0x103)
			{
				model->name = oldModel->name;
				model->animType = oldModel->animType;
				model->animSpeed = oldModel->animSpeed;
				model->blockType = oldModel->blockType;
			}
			model->fileName = oldModel->fileName;
			model->position = oldModel->position / 2.0f;
			model->rotation = oldModel->rotation;
			model->scale = oldModel->scale / 2.0f;
			model->model = newMap->getRsw()->getRsm(model->fileName);
			newMap->getRsw()->objects.push_back(model);
		}

	}

	delete this->map;
	this->map = newMap;
	mapRenderer.setMap(newMap);
	textureWindow->updateTextures(newMap);
	objectWindow->updateObjects(newMap);
}
