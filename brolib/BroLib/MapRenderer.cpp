#include "MapRenderer.h"
#include "Map.h"
#include "Gnd.h"

#include <blib/Shader.h>
#include <blib/ResourceManager.h>
#include <blib/gl/Vertex.h>
#include <blib/Renderer.h>
#include <blib/BackgroundTask.h>
#include <blib/util/Log.h>

using blib::util::Log;

#include <vector>
#include <glm/gtc/matrix_transform.hpp>


void MapRenderer::render( blib::Renderer* renderer )
{
	renderGnd(renderer);
	renderRsw(renderer);
}

void MapRenderer::init( blib::ResourceManager* resourceManager, blib::App* app )
{
	this->resourceManager = resourceManager;
	this->app = app;
	
	gndRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/gnd");
	gndRenderState.activeShader->bindAttributeLocation("a_position", 0);
	gndRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	gndRenderState.activeShader->bindAttributeLocation("a_texture2", 2);
	gndRenderState.activeShader->setUniform("projectionMatrix", glm::perspective(90.0f, 1.0f, 0.01f, 5000.0f));
	gndRenderState.activeShader->setUniform("s_texture", 0);
	gndRenderState.activeShader->setUniform("s_lighting", 1);
	gndRenderState.blendEnabled = true;
	gndRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	gndRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	gndRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.depthTest = true;

	gndShadow = resourceManager->getResource<blib::Texture>(2048, 2048);
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

#pragma region GND

void MapRenderer::renderGnd(blib::Renderer* renderer)
{
	static bool bla = false;
	if(!bla)
	{
		new blib::BackgroundTask<char*>(app, 
			[this, renderer] {
				char* data = new char[2048*2048*4];
				int x = 0; int y = 0;
				for(size_t i = 0; i < map->getGnd()->lightmaps.size(); i++)
				{
					Gnd::Lightmap* lightMap = map->getGnd()->lightmaps[i];
					for(int xx = 0; xx < 8; xx++)
					{
						for(int yy = 0; yy < 8; yy++)
						{
							int xxx = 8*x + xx;
							int yyy = 8*y + yy;

							data[4*(xxx+2048*yyy)+0] = lightMap->data[64+3*(xx+8*yy)+0];
							data[4*(xxx+2048*yyy)+1] = lightMap->data[64+3*(xx+8*yy)+1];
							data[4*(xxx+2048*yyy)+2] = lightMap->data[64+3*(xx+8*yy)+2];
							data[4*(xxx+2048*yyy)+3] = lightMap->data[xx+8*yy];
						}
					}
					x++;
					if(x*8 >= 2048)
					{
						x = 0;
						y++;
					}
				}
				return data;
			}, [renderer, this](char* data)
			{
				renderer->setTextureSubImage(gndShadow, 0, 0, 2048, 2048, data);

			});

		bla = true;
	}
	//load textures if needed
	if(map->getGnd()->textures[0]->texture == NULL)
		for(size_t i = 0; i < map->getGnd()->textures.size(); i++)
			map->getGnd()->textures[i]->texture = resourceManager->getResource<blib::Texture>("data/texture/" + map->getGnd()->textures[i]->file);

	//render gnd chunks
	gndRenderState.activeShader->setUniform("modelViewMatrix", cameraMatrix);
	gndRenderState.activeTexture[1] = gndShadow;
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
	vbo->setVertexFormat<GndVertex>();
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
			renderer->drawTriangles<GndVertex>(it.second.first, it.second.second, gndRenderState);
		}
	}
}

void MapRenderer::GndChunk::rebuild( const Gnd* gnd, blib::App* app, blib::Renderer* renderer )
{
	Log::out<<"Rebuilding chunk "<<x<<", "<<y<<Log::newline;
	rebuilding = true;
	

	struct NewChunkData
	{
		std::vector<GndVertex> allVerts;
		std::map<int, std::pair<int, int> > newVertIndices;
	};

	new blib::BackgroundTask<NewChunkData>(app, [this, gnd, renderer] () {
		std::map<int, std::vector<GndVertex> > verts;
		NewChunkData ret;
		for(int y = this->y; y < glm::min(this->y+CHUNKSIZE, (int)gnd->cubes.size()); y++)
		{
			for(int x = this->x; x < glm::min(this->x+CHUNKSIZE, (int)gnd->cubes[y].size()); x++)
			{
				if(gnd->cubes[y][x]->tileUp != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[y][x]->tileUp];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10*x,	-gnd->cubes[y][x]->h1,10*y),	tile->v1, glm::vec2(lm1.x,lm1.y));
					GndVertex v2(glm::vec3(10*x+10,	-gnd->cubes[y][x]->h3,10*y),	tile->v3, glm::vec2(lm1.x,lm2.y));
					GndVertex v3(glm::vec3(10*x,	-gnd->cubes[y][x]->h2,10*y+10), tile->v2, glm::vec2(lm2.x,lm1.y));
					GndVertex v4(glm::vec3(10*x+10,	-gnd->cubes[y][x]->h4,10*y+10), tile->v4, glm::vec2(lm2.x,lm2.y));

					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
				if(gnd->cubes[y][x]->tileFront != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[y][x]->tileFront];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10*x,	-gnd->cubes[y][x]->h1,	10*y+10),	tile->v2, glm::vec2(lm2.x,lm1.y));
					GndVertex v2(glm::vec3(10*x+10,	-gnd->cubes[y][x]->h3,	10*y+10),	tile->v1, glm::vec2(lm1.x,lm1.y));
					GndVertex v3(glm::vec3(10*x,	-gnd->cubes[y+1][x]->h2,10*y+10),	tile->v4, glm::vec2(lm2.x,lm2.y));
					GndVertex v4(glm::vec3(10*x+10,	-gnd->cubes[y+1][x]->h4,10*y+10),	tile->v3, glm::vec2(lm1.x,lm2.y));
					
					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
				if(gnd->cubes[y][x]->tileSide != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[y][x]->tileSide];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10*x+10,	-gnd->cubes[y][x]->h1,	10*y),		tile->v1, glm::vec2(lm1.x,lm1.y));
					GndVertex v2(glm::vec3(10*x+10,	-gnd->cubes[y][x]->h3,	10*y+10),	tile->v2, glm::vec2(lm2.x,lm1.y));
					GndVertex v3(glm::vec3(10*x+10,	-gnd->cubes[y][x+1]->h2,10*y),		tile->v3, glm::vec2(lm1.x,lm2.y));
					GndVertex v4(glm::vec3(10*x+10,	-gnd->cubes[y][x+1]->h4,10*y+10),	tile->v4, glm::vec2(lm2.x,lm2.y));

					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
			}
		}
		ret.newVertIndices.clear();
		ret.allVerts.clear();
		for(auto it : verts)
		{
			ret.newVertIndices[it.first] = std::pair<int,int>(ret.allVerts.size(), it.second.size());
			ret.allVerts.insert(ret.allVerts.end(), it.second.begin(), it.second.end());
		}

		return ret;
	},
		[this, renderer] (const NewChunkData& data) {
			if(!data.allVerts.empty())
				renderer->setVbo(vbo, data.allVerts);
			vertIndices = data.newVertIndices;
			dirty = false;
			rebuilding = false;
	});

}

#pragma endregion GND


void MapRenderer::renderRsw( blib::Renderer* renderer )
{
}
