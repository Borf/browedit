#include "MapRenderer.h"
#include "Map.h"
#include "Gnd.h"

#include <blib/Shader.h>
#include <blib/ResourceManager.h>
#include <blib/gl/Vertex.h>
#include <blib/Renderer.h>
#include <blib/BackgroundTask.h>

#include <vector>
#include <glm/gtc/matrix_transform.hpp>


void MapRenderer::render( blib::Renderer* renderer )
{
	renderGnd(renderer);
}

void MapRenderer::init( blib::ResourceManager* resourceManager, blib::App* app )
{
	this->resourceManager = resourceManager;
	this->app = app;
	
	gndRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/gnd");
	gndRenderState.activeShader->bindAttributeLocation("a_position", 0);
	gndRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	gndRenderState.activeShader->setUniform("projectionMatrix", glm::perspective(90.0f, 1.0f, 0.01f, 5000.0f));
	gndRenderState.activeShader->setUniform("s_texture", 0);
	gndRenderState.blendEnabled = true;
	gndRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	gndRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	gndRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
}

void MapRenderer::setMap(const Map* map)
{
	this->map = map;

	if(!gndChunks.empty())
	{
		for(size_t y = 0; y < gndChunks.size(); y++)
			for(size_t x = 0; x < gndChunks[y].size(); x++)
				delete gndChunks[y][x];
		gndChunks.clear();
	}

	gndChunks.resize((int)ceil(map->getGnd()->height / (float)CHUNKSIZE), std::vector<GndChunk*>((int)ceil(map->getGnd()->width / (float)CHUNKSIZE), NULL));
	for(size_t y = 0; y < gndChunks.size(); y++)
		for(size_t x = 0; x < gndChunks[y].size(); x++)
			gndChunks[y][x] = new GndChunk(x*CHUNKSIZE,y*CHUNKSIZE, resourceManager);
}


void MapRenderer::renderGnd(blib::Renderer* renderer)
{
	//load textures if needed
	if(map->getGnd()->textures[0]->texture == NULL)
		for(size_t i = 0; i < map->getGnd()->textures.size(); i++)
			map->getGnd()->textures[i]->texture = resourceManager->getResource<blib::Texture>("data/texture/" + map->getGnd()->textures[i]->file);

	//render gnd chunks
	gndRenderState.activeShader->setUniform("modelViewMatrix", cameraMatrix);
	for(auto r : gndChunks)
		for(auto c : r)
			c->render(map->getGnd(), app, gndRenderState, renderer);

}


/////gndchunk

MapRenderer::GndChunk::GndChunk( int x, int y, blib::ResourceManager* resourceManager )
{
	dirty = true;
	rebuilding = false;
	vbo = NULL;
	this->x = x;
	this->y = y;
	vbo = resourceManager->getResource<blib::VBO>();
	vbo->setVertexFormat<blib::VertexP3T2>();
}



void MapRenderer::GndChunk::render( const Gnd* gnd, blib::App* app, blib::RenderState& gndRenderState, blib::Renderer* renderer )
{
	if((dirty || !vbo) && !rebuilding)
		rebuild(gnd, app, renderer);

	if(vbo && !rebuilding)
	{
		gndRenderState.activeVbo = vbo;
		for(auto it : vertIndices)
		{
			gndRenderState.activeTexture[0] = gnd->textures[it.first]->texture;
			renderer->drawTriangles<blib::VertexP3T2>(it.second.first, it.second.second, gndRenderState);
		}
	}
}

void MapRenderer::GndChunk::rebuild( const Gnd* gnd, blib::App* app, blib::Renderer* renderer )
{
	rebuilding = true;
	

	new blib::BackgroundTask(app, [this, gnd, renderer] () {
		std::map<int, std::vector<blib::VertexP3T2> > verts;

		for(int y = this->y; y < glm::min(this->y+CHUNKSIZE, (int)gnd->cubes.size()); y++)
		{
			for(int x = this->x; x < glm::min(this->x+CHUNKSIZE, (int)gnd->cubes[y].size()); x++)
			{
				if(gnd->cubes[y][x]->tileUp != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[y][x]->tileUp];
					blib::VertexP3T2 v1(glm::vec3(10*x,		-gnd->cubes[y][x]->h1,10*y),	tile->v1);
					blib::VertexP3T2 v2(glm::vec3(10*x+10,	-gnd->cubes[y][x]->h3,10*y),	tile->v3);
					blib::VertexP3T2 v3(glm::vec3(10*x,		-gnd->cubes[y][x]->h2,10*y+10), tile->v2);
					blib::VertexP3T2 v4(glm::vec3(10*x+10,	-gnd->cubes[y][x]->h4,10*y+10), tile->v4);

					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
			}
		}

		allVerts.clear();
		for(auto it : verts)
		{
			vertIndices[it.first] = std::pair<int,int>(allVerts.size(), it.second.size());
			allVerts.insert(allVerts.end(), it.second.begin(), it.second.end());
		}
	},
		[this, renderer] () {
			if(!allVerts.empty())
				renderer->setVbo(vbo, allVerts);
			allVerts.clear();
			dirty = false;
			rebuilding = false;
	});

}
