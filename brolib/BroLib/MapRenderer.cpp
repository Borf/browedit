#include "MapRenderer.h"
#include "Map.h"
#include "Gnd.h"
#include "Rsw.h"
#include "Rsm.h"
#include "Renderer.h"

#include <blib/Shader.h>
#include <blib/ResourceManager.h>
#include <blib/gl/Vertex.h>
#include <blib/Renderer.h>
#include <blib/BackgroundTask.h>
#include <blib/util/Log.h>
#include <blib/VBO.h>
#include <blib/Window.h>

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
	

	float fov = 65;

	gndRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/gnd");
	gndRenderState.activeShader->bindAttributeLocation("a_position", 0);
	gndRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	gndRenderState.activeShader->bindAttributeLocation("a_texture2", 2);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::ModelViewMatrix, "modelViewMatrix", blib::Shader::Mat4);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::s_texture, "s_texture", blib::Shader::Int);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::s_lighting, "s_lighting", blib::Shader::Int);
	gndRenderState.activeShader->finishUniformSetup();

	gndRenderState.activeShader->setUniform(GndShaderAttributes::ProjectionMatrix, glm::perspective(fov, app->window->getWidth() / (float)app->window->getHeight(), 0.1f, 5000.0f));
	gndRenderState.activeShader->setUniform(GndShaderAttributes::s_texture, 0);
	gndRenderState.activeShader->setUniform(GndShaderAttributes::s_lighting, 1);
	gndRenderState.blendEnabled = true;
	gndRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	gndRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	gndRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.depthTest = true;

	gndShadow = resourceManager->getResource<blib::Texture>(2048, 2048);


	rswRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/rsw");
	rswRenderState.activeShader->bindAttributeLocation("a_position", 0);
	rswRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::CameraMatrix, "cameraMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ModelMatrix, "modelMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::s_texture, "s_texture", blib::Shader::Int);
	rswRenderState.activeShader->finishUniformSetup();

	rswRenderState.activeShader->setUniform(RswShaderAttributes::ProjectionMatrix, glm::perspective(fov, app->window->getWidth() / (float)app->window->getHeight(), 0.1f, 5000.0f));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::s_texture, 0);
	rswRenderState.blendEnabled = true;
	rswRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	rswRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	rswRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	rswRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	rswRenderState.depthTest = true;

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
	gndRenderState.activeShader->setUniform(GndShaderAttributes::ModelViewMatrix, cameraMatrix);
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
		for (VboIndex& it : vertIndices)
		{
			gndRenderState.activeTexture[0] = gnd->textures[it.texture]->texture;
			renderer->drawTriangles<GndVertex>(it.begin, it.count, gndRenderState);
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
		std::vector<VboIndex> newVertIndices;
	};

	new blib::BackgroundTask<NewChunkData>(app, [this, gnd, renderer] () {
		std::map<int, std::vector<GndVertex> > verts;
		NewChunkData ret;
		for(int x = this->x; x < glm::min(this->x+CHUNKSIZE, (int)gnd->cubes.size()); x++)
		{
			for(int y = this->y; y < glm::min(this->y+CHUNKSIZE, (int)gnd->cubes[x].size()); y++)
			{
				if(gnd->cubes[x][y]->tileUp != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[x][y]->tileUp];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10*x,	-gnd->cubes[x][y]->h3,10*gnd->height-10*y),	tile->v3,		glm::vec2(lm1.x,lm2.y));
					GndVertex v2(glm::vec3(10*x+10,	-gnd->cubes[x][y]->h4,10*gnd->height-10*y),	tile->v4,		glm::vec2(lm2.x,lm2.y));
					GndVertex v3(glm::vec3(10*x,	-gnd->cubes[x][y]->h1,10*gnd->height-10*y+10), tile->v1,	glm::vec2(lm1.x,lm1.y));
					GndVertex v4(glm::vec3(10*x+10,	-gnd->cubes[x][y]->h2,10*gnd->height-10*y+10), tile->v2,	glm::vec2(lm2.x,lm1.y));

					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
				if(gnd->cubes[x][y]->tileFront != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[x][y]->tileFront];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10 * x + 10, -gnd->cubes[x][y]->h1,		10 * gnd->height - 10 * y),			tile->v1, glm::vec2(lm1.x, lm1.y));
					GndVertex v2(glm::vec3(10 * x + 10, -gnd->cubes[x][y]->h3,		10 * gnd->height - 10 * y + 10),	tile->v2, glm::vec2(lm2.x, lm1.y));
					GndVertex v3(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h2,	10 * gnd->height - 10 * y),			tile->v3, glm::vec2(lm1.x, lm2.y));
					GndVertex v4(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h4,	10 * gnd->height - 10 * y + 10),	tile->v4, glm::vec2(lm2.x, lm2.y));
					
					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
				if(gnd->cubes[x][y]->tileSide != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[x][y]->tileSide];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10*x,	-gnd->cubes[x][y]->h1,	10*gnd->height-10*y),	tile->v1, glm::vec2(lm1.x,lm1.y));
					GndVertex v2(glm::vec3(10*x+10,	-gnd->cubes[x][y]->h3,	10*gnd->height-10*y),	tile->v2, glm::vec2(lm2.x,lm1.y));
					GndVertex v3(glm::vec3(10*x,	-gnd->cubes[x][y+1]->h2,10*gnd->height-10*y),	tile->v3, glm::vec2(lm1.x,lm2.y));
					GndVertex v4(glm::vec3(10*x+10,	-gnd->cubes[x][y+1]->h4,10*gnd->height-10*y),	tile->v4, glm::vec2(lm2.x,lm2.y));

					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
			}
		}
		ret.newVertIndices.clear();
		ret.allVerts.clear();
		for(auto it : verts)
		{
			ret.newVertIndices.push_back(VboIndex(it.first, ret.allVerts.size(), it.second.size()));
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

#pragma endregion


#pragma region RSW

void MapRenderer::renderRsw( blib::Renderer* renderer )
{
	rswRenderState.activeShader->setUniform(RswShaderAttributes::CameraMatrix, cameraMatrix);
	rswRenderState.activeTexture[1] = gndShadow;

//	renderer->setShaderState(rswRenderState.activeShader);
//	rswRenderState.activeShader->state.clear();
	for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
	{
		if (map->getRsw()->objects[i]->type == Rsw::Object::Type::Model)
		{
			renderModel(static_cast<Rsw::Model*>(map->getRsw()->objects[i]), renderer);
		}
	}


}

void MapRenderer::renderModel(Rsw::Model* model, blib::Renderer* renderer)
{
	glm::mat4 matrix;
	if (!model->matrixCached)
	{
		model->matrixCache = glm::mat4();
		model->matrixCache = glm::scale(model->matrixCache, glm::vec3(1, 1, -1));
		model->matrixCache = glm::translate(model->matrixCache, glm::vec3(5 * map->getGnd()->width + model->position.x, -model->position.y, -10-5 * map->getGnd()->height + model->position.z));
		model->matrixCache = glm::rotate(model->matrixCache, -model->rotation.z, glm::vec3(0, 0, 1));
		model->matrixCache = glm::rotate(model->matrixCache, -model->rotation.x, glm::vec3(1, 0, 0));
		model->matrixCache = glm::rotate(model->matrixCache, model->rotation.y, glm::vec3(0, 1, 0));
		model->matrixCache = glm::scale(model->matrixCache, glm::vec3(model->scale.x, -model->scale.y, model->scale.z));
		model->matrixCache = glm::translate(model->matrixCache, glm::vec3(-model->model->realbbrange.x, model->model->realbbmin.y, -model->model->realbbrange.z));
		model->matrixCached = true;
	}

	if (model->model->renderer == NULL)
	{
		model->model->renderer = new RsmModelRenderInfo();
		for (size_t i = 0; i < model->model->textures.size(); i++)
			model->model->renderer->textures.push_back(resourceManager->getResource<blib::Texture>("data/texture/" + model->model->textures[i]));
	}
	renderMesh(model->model->rootMesh, model->matrixCache, model->model->renderer, renderer);
}

void MapRenderer::renderMesh(Rsm::Mesh* mesh, glm::mat4 matrix, RsmModelRenderInfo* renderInfo, blib::Renderer* renderer)
{
	matrix *= mesh->matrix1;
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix, matrix * mesh->matrix2);

	if (mesh->renderer == NULL)
	{
		mesh->renderer = new RsmMeshRenderInfo();
		mesh->renderer->vbo = resourceManager->getResource<blib::VBO>();
		mesh->renderer->vbo->setVertexFormat<blib::VertexP3T2>();

		std::map<int, std::vector<blib::VertexP3T2> > verts;
		for (size_t i = 0; i < mesh->faces.size(); i++)
		{
			for (int ii = 0; ii < 3; ii++)
				verts[mesh->faces[i]->texIndex].push_back(blib::VertexP3T2(mesh->vertices[mesh->faces[i]->vertices[ii]], mesh->texCoords[mesh->faces[i]->texvertices[ii]]));
		}

		std::vector<blib::VertexP3T2> allVerts;
		for (std::map<int, std::vector<blib::VertexP3T2> >::iterator it2 = verts.begin(); it2 != verts.end(); it2++)
		{
			mesh->renderer->indices.push_back(VboIndex(it2->first, allVerts.size(), it2->second.size()));
			allVerts.insert(allVerts.end(), it2->second.begin(), it2->second.end());
		}

		renderer->setVbo(mesh->renderer->vbo, allVerts);
	}
	RsmMeshRenderInfo* meshInfo = mesh->renderer;

	rswRenderState.activeVbo = meshInfo->vbo;

	for (VboIndex& it : meshInfo->indices)
	{
		rswRenderState.activeTexture[0] = renderInfo->textures[mesh->textures[it.texture]];
		renderer->drawTriangles<blib::VertexP3T2>(it.begin, it.count, rswRenderState);
	}


}


#pragma endregion