#include "MapRenderer.h"
#include "Map.h"
#include "Gnd.h"

#include <blib/Shader.h>
#include <blib/ResourceManager.h>
#include <blib/gl/Vertex.h>
#include <blib/Renderer.h>

#include <vector>
#include <glm/gtc/matrix_transform.hpp>


void MapRenderer::render( blib::Renderer* renderer, const Map* map )
{
	renderGnd(renderer,map);
}

void MapRenderer::init( blib::ResourceManager* resourceManager )
{
	this->resourceManager = resourceManager;
	
	gndRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/gnd");
	gndRenderState.activeShader->bindAttributeLocation("a_position", 0);
	gndRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	gndRenderState.activeShader->setUniform("projectionMatrix", glm::perspective(80.0f, 1.0f, 0.01f, 5000.0f));
	gndRenderState.activeShader->setUniform("s_texture", 0);
	gndRenderState.blendEnabled = true;
	gndRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	gndRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	gndRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
}



void MapRenderer::renderGnd(blib::Renderer* renderer, const Map* map)
{
	std::map<int, std::vector<blib::VertexP3T2> > verts;

	for(size_t y = 0; y < map->getGnd()->cubes.size(); y++)
	{
		for(size_t x = 0; x < map->getGnd()->cubes[y].size(); x++)
		{
			if(map->getGnd()->cubes[y][x]->tileUp != -1)
			{
				Gnd::Tile* tile = map->getGnd()->tiles[map->getGnd()->cubes[y][x]->tileUp];
				blib::VertexP3T2 v1(glm::vec3(10*x,		-map->getGnd()->cubes[y][x]->h1,10*y),	tile->v1);
				blib::VertexP3T2 v2(glm::vec3(10*x+10,	-map->getGnd()->cubes[y][x]->h3,10*y),	tile->v3);
				blib::VertexP3T2 v3(glm::vec3(10*x,		-map->getGnd()->cubes[y][x]->h2,10*y+10), tile->v2);
				blib::VertexP3T2 v4(glm::vec3(10*x+10,	-map->getGnd()->cubes[y][x]->h4,10*y+10), tile->v4);

				verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
				verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
			}
		}
	}
	if(map->getGnd()->textures[0]->texture == NULL)
		for(size_t i = 0; i < map->getGnd()->textures.size(); i++)
			map->getGnd()->textures[i]->texture = resourceManager->getResource<blib::Texture>("data/texture/" + map->getGnd()->textures[i]->file);

	gndRenderState.activeShader->setUniform("modelViewMatrix", cameraMatrix);
//	gndRenderState.renderStyle = blib::RenderState::WIREFRAME;


	for(auto it : verts)
	{
		gndRenderState.activeTexture[0] = map->getGnd()->textures[it.first]->texture;
		renderer->drawTriangles(it.second, gndRenderState);

	}

}