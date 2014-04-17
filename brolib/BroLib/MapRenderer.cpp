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
#include <blib/FBO.h>

using blib::util::Log;

#include <vector>
#include <glm/gtc/matrix_transform.hpp>


MapRenderer::MapRenderer()
{
	drawShadows = true;
	drawObjects = true;
	drawLights = true;
	drawEffects = true;
	drawSounds = true;
	drawTextureGrid = true;
	drawObjectGrid = true;
	drawQuadTree = true;

}

float mod(float x, float m)
{
	while (x > m)
		x -= m;
	while (x < 0)
		x += m;
	return x;
}
glm::vec2 mod(const glm::vec2 &a, float m)
{
	return glm::vec2(mod(a.x, m), mod(a.y, m));
}
float dist(const glm::vec2 &a, const glm::vec2 &b)
{
	return glm::length(glm::min(mod(a - b, 1), mod(b - a, 1)));
}


void MapRenderer::render(blib::Renderer* renderer, glm::vec2 mousePosition)
{
	renderer->clear(glm::vec4(0, 0, 0, 1), blib::Renderer::Color | blib::Renderer::Depth, gndRenderState);
	renderGnd(renderer);
	renderer->unproject(mousePosition, &mouse3d, cameraMatrix, projectionMatrix);


	if (drawTextureGrid)
	{
		if (gndGridDirty)
		{
			std::vector<blib::VertexP3> verts;
			Gnd* gnd = map->getGnd();

			for (int x = 1; x < gnd->width-1; x++)
			{
				for (int y = 1; y < gnd->height-1; y++)
				{
					{
						Gnd::Tile* t1 = NULL;
						Gnd::Tile* t2 = NULL;
						if (gnd->cubes[x][y]->tileUp != -1)
							t1 = gnd->tiles[gnd->cubes[x][y]->tileUp];
						if (gnd->cubes[x+1][y]->tileUp != -1)
							t2 = gnd->tiles[gnd->cubes[x+1][y]->tileUp];

						bool drawLine = false;
						if ((t1 == NULL) != (t2 == NULL)) // NULL next to a tile
							drawLine = true;
						else if (t1 == NULL)
							drawLine = false; // both tiles are NULL
						else if (t1->textureIndex != t2->textureIndex) // 2 different textures
							drawLine = true;
						else if (dist(t1->v2, t2->v1) > 0.1 || dist(t1->v4, t2->v3) > 0.1)
							drawLine = true;

						if (drawLine)
						{
							verts.push_back(blib::VertexP3(glm::vec3(10 * x+10, -gnd->cubes[x][y]->h4 + 0.1f, 10 * gnd->height - 10 * y)));
							verts.push_back(blib::VertexP3(glm::vec3(10 * x+10, -gnd->cubes[x][y]->h2 + 0.1f, 10 * gnd->height - 10 * y + 10)));
						}
					}

					{
						Gnd::Tile* t1 = NULL;
						Gnd::Tile* t2 = NULL;
						if (gnd->cubes[x][y]->tileUp != -1)
							t1 = gnd->tiles[gnd->cubes[x][y]->tileUp];
						if (gnd->cubes[x - 1][y]->tileUp != -1)
							t2 = gnd->tiles[gnd->cubes[x - 1][y]->tileUp];

						bool drawLine = false;
						if ((t1 == NULL) != (t2 == NULL)) // NULL next to a tile
							drawLine = true;
						else if (t1 == NULL)
							drawLine = false; // both tiles are NULL
						else if (t1->textureIndex != t2->textureIndex) // 2 different textures
							drawLine = true;
						else if (dist(t1->v1, t2->v2) > 0.1 || dist(t1->v3, t2->v4) > 0.1)
							drawLine = true;

						if (drawLine)
						{
							verts.push_back(blib::VertexP3(glm::vec3(10 * x, -gnd->cubes[x][y]->h3 + 0.1f, 10 * gnd->height - 10 * y)));
							verts.push_back(blib::VertexP3(glm::vec3(10 * x, -gnd->cubes[x][y]->h1 + 0.1f, 10 * gnd->height - 10 * y + 10)));
						}
					}

					{
						Gnd::Tile* t1 = NULL;
						Gnd::Tile* t2 = NULL;
						if (gnd->cubes[x][y]->tileUp != -1)
							t1 = gnd->tiles[gnd->cubes[x][y]->tileUp];
						if (gnd->cubes[x][y + 1]->tileUp != -1)
							t2 = gnd->tiles[gnd->cubes[x][y + 1]->tileUp];

						bool drawLine = false;
						if ((t1 == NULL) != (t2 == NULL)) // NULL next to a tile
							drawLine = true;
						else if (t1 == NULL)
							drawLine = false; // both tiles are NULL
						else if (t1->textureIndex != t2->textureIndex) // 2 different textures
							drawLine = true;
						else if (dist(t1->v3, t2->v1) > 0.1 || dist(t1->v4, t2->v2) > 0.1)
							drawLine = true;

						if (drawLine)
						{
							verts.push_back(blib::VertexP3(glm::vec3(10 * x, -gnd->cubes[x][y]->h3 + 0.1f, 10 * gnd->height - 10 * y)));
							verts.push_back(blib::VertexP3(glm::vec3(10 * x + 10, -gnd->cubes[x][y]->h4 + 0.1f, 10 * gnd->height - 10 * y)));
						}
					}
					
					{
						Gnd::Tile* t1 = NULL;
						Gnd::Tile* t2 = NULL;
						if (gnd->cubes[x][y]->tileUp != -1)
							t1 = gnd->tiles[gnd->cubes[x][y]->tileUp];
						if (gnd->cubes[x][y - 1]->tileUp != -1)
							t2 = gnd->tiles[gnd->cubes[x][y - 1]->tileUp];

						bool drawLine = false;
						if ((t1 == NULL) != (t2 == NULL)) // NULL next to a tile
							drawLine = true;
						else if (t1 == NULL)
							drawLine = false; // both tiles are NULL
						else if (t1->textureIndex != t2->textureIndex) // 2 different textures
							drawLine = true;
						else if (dist(t1->v1, t2->v3) > 0.1 || dist(t1->v2, t2->v4) > 0.1)
							drawLine = true;

						if (drawLine)
						{
							verts.push_back(blib::VertexP3(glm::vec3(10 * x, -gnd->cubes[x][y]->h1 + 0.1f, 10 * gnd->height - 10 * y + 10)));
							verts.push_back(blib::VertexP3(glm::vec3(10 * x + 10, -gnd->cubes[x][y]->h2 + 0.1f, 10 * gnd->height - 10 * y + 10)));
						}
					}
				}
			}
			renderer->setVbo(gndTextureGridVbo, verts);
			gndGridDirty = false;
		}
		//highlightRenderState.depthTest = false;
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, cameraMatrix);
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, projectionMatrix);
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(1, 0, 0, 1));
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
		highlightRenderState.activeTexture[0] = NULL;

		highlightRenderState.activeVbo = gndTextureGridVbo;
		renderer->drawLines<blib::VertexP3>(gndTextureGridVbo->getLength(), highlightRenderState);
		highlightRenderState.activeVbo = NULL;

	}

	renderRsw(renderer);

}

void MapRenderer::init( blib::ResourceManager* resourceManager, blib::App* app )
{
	this->resourceManager = resourceManager;
	this->app = app;
	
	fbo = resourceManager->getResource<blib::FBO>();
	fbo->setSize(app->window->getWidth(), app->window->getHeight());
	fbo->depth = true;
	fbo->textureCount = 2;
	fbo->stencil = false;

	gndRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/gnd");
	gndRenderState.activeShader->bindAttributeLocation("a_position", 0);
	gndRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	gndRenderState.activeShader->bindAttributeLocation("a_texture2", 2);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::ModelViewMatrix, "modelViewMatrix", blib::Shader::Mat4);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::s_texture, "s_texture", blib::Shader::Int);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::s_lighting, "s_lighting", blib::Shader::Int);
	gndRenderState.activeShader->finishUniformSetup();

	gndRenderState.activeShader->setUniform(GndShaderAttributes::s_texture, 0);
	gndRenderState.activeShader->setUniform(GndShaderAttributes::s_lighting, 1);
	gndRenderState.activeFbo = fbo;
	gndRenderState.blendEnabled = true;
	gndRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	gndRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	gndRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.depthTest = true;

	gndShadow = resourceManager->getResource<blib::Texture>(2048, 2048);
	gndNoShadow = resourceManager->getResource<blib::Texture>(1,1);
	gndTextureGridVbo = resourceManager->getResource<blib::VBO>();
	gndTextureGridVbo->setVertexFormat<blib::VertexP3>();


	rswRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/rsw");
	rswRenderState.activeShader->bindAttributeLocation("a_position", 0);
	rswRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::CameraMatrix, "cameraMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ModelMatrix, "modelMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ModelMatrix2, "modelMatrix2", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::s_texture, "s_texture", blib::Shader::Int);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::highlightColor, "highlightColor", blib::Shader::Vec4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::billboard, "billboard", blib::Shader::Float);
	rswRenderState.activeShader->finishUniformSetup();

	rswRenderState.activeShader->setUniform(RswShaderAttributes::s_texture, 0);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::billboard, 0.0f);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::highlightColor, glm::vec4(0, 0, 0, 0));
	rswRenderState.activeFbo = fbo;
	rswRenderState.blendEnabled = true;
	rswRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	rswRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	rswRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	rswRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	rswRenderState.depthTest = true;

	highlightRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/highlight");
	highlightRenderState.activeShader->bindAttributeLocation("a_position", 0);
	highlightRenderState.activeShader->bindAttributeLocation("a_texcoord", 1);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::s_texture, "s_texture", blib::Shader::Int);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::color, "color", blib::Shader::Vec4);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::texMult, "texMult", blib::Shader::Vec4);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::modelviewMatrix, "modelviewMatrix", blib::Shader::Mat4);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::projectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	highlightRenderState.activeShader->finishUniformSetup();
	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::s_texture, 0);
	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));

	highlightRenderState.activeFbo = fbo;
	highlightRenderState.depthTest = true;
	highlightRenderState.blendEnabled = true;
	highlightRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	highlightRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	highlightRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	highlightRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;


	rswLightTexture = resourceManager->getResource<blib::Texture>("assets/light.png");
	rswEffectTexture= resourceManager->getResource<blib::Texture>("assets/effect.png");
	rswSoundTexture = resourceManager->getResource<blib::Texture>("assets/sound.png");

	gndShadowDirty = false;
	gndGridDirty = false;
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
	gndShadowDirty = true;
	gndGridDirty = true;


	//load textures if needed
	if (map->getGnd()->textures[0]->texture == NULL)
		for (size_t i = 0; i < map->getGnd()->textures.size(); i++)
			map->getGnd()->textures[i]->texture = resourceManager->getResource<blib::Texture>("data/texture/" + map->getGnd()->textures[i]->file);
}

#pragma region GND

void MapRenderer::renderGnd(blib::Renderer* renderer)
{
	if (gndShadowDirty)
	{
		char shadowData[4] = { 0, 0, 0, 0xffu };
		renderer->setTextureSubImage(gndNoShadow, 0, 0, 1, 1, shadowData);

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

			gndShadowDirty = false;
	}

	//render gnd chunks
	gndRenderState.activeShader->setUniform(GndShaderAttributes::ModelViewMatrix, cameraMatrix);
	if (drawShadows)
		gndRenderState.activeTexture[1] = gndShadow;
	else
		gndRenderState.activeTexture[1] = gndNoShadow;
	for (auto r : gndChunks)
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

	if(vbo)
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

					GndVertex v1(glm::vec3(10 * x + 10, -gnd->cubes[x][y]->h2,		10 * gnd->height - 10 * y + 10),	tile->v2, glm::vec2(lm2.x, lm1.y));
					GndVertex v2(glm::vec3(10 * x + 10, -gnd->cubes[x][y]->h4,		10 * gnd->height - 10 * y),			tile->v1, glm::vec2(lm1.x, lm1.y));
					GndVertex v3(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h1,	10 * gnd->height - 10 * y + 10),	tile->v4, glm::vec2(lm2.x, lm2.y));
					GndVertex v4(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h3,	10 * gnd->height - 10 * y),			tile->v3, glm::vec2(lm1.x, lm2.y));
					
					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
				if(gnd->cubes[x][y]->tileSide != -1)
				{
					Gnd::Tile* tile = gnd->tiles[gnd->cubes[x][y]->tileSide];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10*x,	-gnd->cubes[x][y]->h3,	10*gnd->height-10*y),	tile->v1, glm::vec2(lm1.x,lm1.y));
					GndVertex v2(glm::vec3(10*x+10,	-gnd->cubes[x][y]->h4,	10*gnd->height-10*y),	tile->v2, glm::vec2(lm2.x,lm1.y));
					GndVertex v4(glm::vec3(10*x+10, -gnd->cubes[x][y+1]->h2,10*gnd->height-10*y),	tile->v4, glm::vec2(lm2.x,lm2.y));
					GndVertex v3(glm::vec3(10*x,    -gnd->cubes[x][y+1]->h1,10*gnd->height-10*y),	tile->v3, glm::vec2(lm1.x,lm2.y));

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
	//rswRenderState.activeTexture[1] = gndShadow;

//	renderer->setShaderState(rswRenderState.activeShader);
//	rswRenderState.activeShader->state.clear();

	//selected objects need to be drawn first
	rswRenderState.activeShader->setUniform(RswShaderAttributes::highlightColor, glm::vec4(1, 1, 1, 1));
	renderObjects(renderer, true);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::highlightColor, glm::vec4(0, 0, 0, 0));
	renderObjects(renderer, false);


}

void MapRenderer::renderModel(Rsw::Model* model, blib::Renderer* renderer)
{
	if (!model)
		return;
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

	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix2, model->matrixCache);
	renderMesh(model->model->rootMesh, glm::mat4(), model->model->renderer, renderer);
}

void MapRenderer::renderMesh(Rsm::Mesh* mesh, const glm::mat4 &matrix, RsmModelRenderInfo* renderInfo, blib::Renderer* renderer)
{
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
		mesh->renderer->matrix = matrix * mesh->matrix1 * mesh->matrix2;
		mesh->renderer->matrixSub = matrix * mesh->matrix1;
	}
	if (!mesh->frames.empty())
	{
		mesh->calcMatrix1();
		mesh->renderer->matrix = matrix * mesh->matrix1 * mesh->matrix2;
		mesh->renderer->matrixSub = matrix * mesh->matrix1;
	}

	RsmMeshRenderInfo* meshInfo = mesh->renderer;

	rswRenderState.activeVbo = meshInfo->vbo;
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix, meshInfo->matrix);

	for (VboIndex& it : meshInfo->indices)
	{
		rswRenderState.activeTexture[0] = renderInfo->textures[mesh->textures[it.texture]];
		renderer->drawTriangles<blib::VertexP3T2>(it.begin, it.count, rswRenderState);
	}

	for (size_t i = 0; i < mesh->children.size(); i++)
		renderMesh(mesh->children[i], meshInfo->matrixSub, renderInfo, renderer);


}

void MapRenderer::resizeGl(int width, int height)
{
	//fbo->setSize(width, height);
	projectionMatrix = glm::perspective(fov, width / (float)height, 5.0f, 5000.0f);
	if (gndRenderState.activeShader)
		gndRenderState.activeShader->setUniform(GndShaderAttributes::ProjectionMatrix, projectionMatrix);
	if (rswRenderState.activeShader)
		rswRenderState.activeShader->setUniform(RswShaderAttributes::ProjectionMatrix, projectionMatrix);
}

void MapRenderer::setTileDirty(int xx, int yy)
{
	gndChunks[yy / CHUNKSIZE][xx / CHUNKSIZE]->dirty = true;
	gndGridDirty = true;
}

void MapRenderer::renderObjects(blib::Renderer* renderer, bool selected)
{
	for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
	{
		Rsw::Object* o = map->getRsw()->objects[i];
		if (o->selected != selected)
			continue;
		if (o->type == Rsw::Object::Type::Model)
		{
			if (drawObjects)
				renderModel(static_cast<Rsw::Model*>(o), renderer);
		}
		else
		{
			blib::Texture* t = NULL;
			if (o->type == Rsw::Object::Type::Light)
				t = rswLightTexture;
			else if (o->type == Rsw::Object::Type::Effect)
				t = rswEffectTexture;
			else if (o->type == Rsw::Object::Type::Sound)
				t = rswSoundTexture;
			else
			{
				Log::err << "Unknown rsw object type" << Log::newline;
				continue;
			}


			if (!o->matrixCached)
			{
				o->matrixCache = glm::mat4();
				o->matrixCache = glm::scale(o->matrixCache, glm::vec3(1, 1, -1));
				o->matrixCache = glm::translate(o->matrixCache, glm::vec3(5 * map->getGnd()->width + o->position.x, -o->position.y, -10 - 5 * map->getGnd()->height + o->position.z));
				o->matrixCache = glm::rotate(o->matrixCache, -o->rotation.z, glm::vec3(0, 0, 1));
				o->matrixCache = glm::rotate(o->matrixCache, -o->rotation.x, glm::vec3(1, 0, 0));
				o->matrixCache = glm::rotate(o->matrixCache, o->rotation.y, glm::vec3(0, 1, 0));
			//	o->matrixCache = glm::scale(o->matrixCache, glm::vec3(o->scale.x, -o->scale.y, o->scale.z));
			//	o->matrixCache = glm::translate(o->matrixCache, glm::vec3(-o->model->realbbrange.x, model->model->realbbmin.y, -model->model->realbbrange.z));
				o->matrixCached = true;
			}


			static blib::VertexP3T2 verts[6] =
			{
				blib::VertexP3T2(glm::vec3(-5, -5, 0), glm::vec2(0, 0)),
				blib::VertexP3T2(glm::vec3(5, -5, 0), glm::vec2(1, 0)),
				blib::VertexP3T2(glm::vec3(-5, 5, 0), glm::vec2(0, 1)),

				blib::VertexP3T2(glm::vec3(5, 5, 0), glm::vec2(1, 1)),
				blib::VertexP3T2(glm::vec3(5, -5, 0), glm::vec2(1, 0)),
				blib::VertexP3T2(glm::vec3(-5, 5, 0), glm::vec2(0, 1)),
			};

			rswRenderState.activeVbo = NULL;
			rswRenderState.activeTexture[0] = t;
			rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix, glm::mat4());
			rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix2, o->matrixCache);
			rswRenderState.activeShader->setUniform(RswShaderAttributes::billboard, 1.0f);
			renderer->drawTriangles(verts, 6, rswRenderState);
			rswRenderState.activeShader->setUniform(RswShaderAttributes::billboard, 0.0f);


		}
	}
}



template class blib::BackgroundTask<char*>;

#pragma endregion