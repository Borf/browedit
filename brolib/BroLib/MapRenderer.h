#pragma once

#include <glm/glm.hpp>
#include <blib/RenderState.h>
#include <blib/gl/Vertex.h>
#include <map>
#include <vector>

namespace blib { class ResourceManager; class Shader; class Renderer; class App; };
class Map;
class Gnd;

#define CHUNKSIZE 64

class MapRenderer
{
private:
	class GndChunk
	{
	public:
		bool dirty;
		bool rebuilding;
		blib::VBO* vbo;
		std::map<int, std::pair<int, int> > vertIndices;
				std::vector<blib::VertexP3T2> allVerts;

		int x, y;


		GndChunk(int x, int y, blib::ResourceManager* resourceManager);

		void render(const Gnd* gnd, blib::App* app, blib::RenderState &gndRenderState, blib::Renderer* renderer);
		void rebuild(const Gnd* gnd, blib::App* app, blib::Renderer* renderer);
	};
	blib::ResourceManager* resourceManager;

	const Map* map;
	blib::App* app;

//gnd
	blib::RenderState gndRenderState;
	std::vector<std::vector<GndChunk*> > gndChunks;
public:
	glm::mat4 cameraMatrix;

	bool drawShadows;
	bool drawObjects;
	bool drawTextureGrid;
	bool drawObjectGrid;
	bool drawQuadTree;

	void init( blib::ResourceManager* resourceManager, blib::App* app );
	void setMap(const Map* map);

	void render(blib::Renderer* renderer);
	void renderGnd(blib::Renderer* renderer);


};
