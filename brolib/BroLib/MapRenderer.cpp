#include "MapRenderer.h"
#include "Map.h"
#include "Gnd.h"
#include "Rsw.h"
#include "Rsm.h"
#include "Gat.h"
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
#include <blib/Shapes.h>
#include <blib/util/Profiler.h>
#include <blib/math/Plane.h>
#include <blib/math/Ray.h>

using blib::util::Log;

#include <vector>
#include <glm/gtc/matrix_transform.hpp>


MapRenderer::MapRenderer() : mouseRay(glm::vec3(0, 0,0), glm::vec3(1,0,0))
{
	drawShadows = true;
	drawObjects = true;
	drawLights = true;
	drawEffects = true;
	drawSounds = true;
	drawTextureGrid = true;
	drawObjectGrid = true;
	drawQuadTree = true;
	fbo = NULL;
	fov = glm::radians(75.0f);
	mouse3d = glm::vec4(0, 0, 0, -1);
	orthoDistance = 1000;
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
	return glm::length(a - b);
	//return glm::length(glm::min(mod(a - b, 1), mod(b - a, 1)));
}


void MapRenderer::render(blib::Renderer* renderer, glm::vec2 mousePosition)
{
	renderer->clear(glm::vec4(0, 0, 0, 0), blib::Renderer::Color | blib::Renderer::Depth, gndRenderState);

	this->resizeGl(width, height, 0, 0); //meh

	glm::vec3 lightDirection;
	lightDirection[0] = -glm::cos(glm::radians((float)map->getRsw()->light.longitude)) * glm::sin(glm::radians((float)map->getRsw()->light.latitude));
	lightDirection[1] = glm::cos(glm::radians((float)map->getRsw()->light.latitude));
	lightDirection[2] = glm::sin(glm::radians((float)map->getRsw()->light.longitude)) * glm::sin(glm::radians((float)map->getRsw()->light.latitude));
	gndRenderState.activeShader->setUniform(GndShaderAttributes::lightAmbient, map->getRsw()->light.ambient);
	gndRenderState.activeShader->setUniform(GndShaderAttributes::lightDiffuse, map->getRsw()->light.diffuse);
	gndRenderState.activeShader->setUniform(GndShaderAttributes::lightIntensity, map->getRsw()->light.intensity);
	gndRenderState.activeShader->setUniform(GndShaderAttributes::lightDirection, lightDirection);

	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightAmbient, map->getRsw()->light.ambient);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDiffuse, map->getRsw()->light.diffuse);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightIntensity, map->getRsw()->light.intensity);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDirection, lightDirection);

	renderGnd(renderer);
	renderer->unproject(mousePosition, &mouse3d, &mouseRay, cameraMatrix, projectionMatrix);
	if (mouse3d.w >= 1)
	{
		float t = 0;
		mouseRay.planeIntersection(blib::math::Plane(glm::vec3(0, 1, 0), 0), t);
		glm::vec3 pos = mouseRay.origin + t * mouseRay.dir;
		mouse3d = glm::vec4(pos, 0.5f);
	}

	if (drawTextureGrid)
	{
		if (gndTextureGridDirty)
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
			gndTextureGridDirty = false;
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


	if (drawObjectGrid)
	{
		if (gndGridDirty)
		{
			std::vector<blib::VertexP3> verts;
			Gnd* gnd = map->getGnd();

			for (int x = 0; x < gnd->width; x++)
			{
				for (int y = 0; y < gnd->height; y++)
				{
					Gnd::Cube* cube = gnd->cubes[x][y];

					blib::VertexP3 v1(glm::vec3(10 * x,			-cube->h3 + 0.1f, 10 * gnd->height - 10 * y));
					blib::VertexP3 v2(glm::vec3(10 * x + 10,	-cube->h4 + 0.1f, 10 * gnd->height - 10 * y));
					blib::VertexP3 v3(glm::vec3(10 * x,			-cube->h1 + 0.1f, 10 * gnd->height - 10 * y + 10));
					blib::VertexP3 v4(glm::vec3(10 * x + 10,	-cube->h2 + 0.1f, 10 * gnd->height - 10 * y + 10));

					verts.push_back(v1);
					verts.push_back(v2);

					verts.push_back(v3);
					verts.push_back(v4);

					verts.push_back(v1);
					verts.push_back(v3);

					verts.push_back(v2);
					verts.push_back(v4);
#if 0 //show normal debug
					glm::vec3 center = (v1.position + v2.position + v3.position + v4.position) / 4.0f;
					verts.push_back(center);
					verts.push_back(center + cube->normal* glm::vec3(5, -5, 5));

					verts.push_back(v1);
					verts.push_back(v1.position + cube->normals[2] * glm::vec3(3, -3, 3));

					verts.push_back(v2);
					verts.push_back(v2.position + cube->normals[3] * glm::vec3(3, -3, 3));

					verts.push_back(v3);
					verts.push_back(v3.position + cube->normals[0] * glm::vec3(3, -3, 3));

					verts.push_back(v4);
					verts.push_back(v4.position + cube->normals[1] * glm::vec3(3, -3, 3));
#endif
				}
			}

			gndGridDirty = false;
			renderer->setVbo(gndGridVbo, verts);
		}
		//highlightRenderState.depthTest = false;
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, cameraMatrix);
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, projectionMatrix);
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(1, 0, 0, 1));
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
		highlightRenderState.activeTexture[0] = NULL;

		highlightRenderState.activeVbo = gndGridVbo;
		renderer->drawLines<blib::VertexP3>(gndGridVbo->getLength(), highlightRenderState);
		highlightRenderState.activeVbo = NULL;


	}



	renderRsw(renderer);

	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, cameraMatrix);
	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, projectionMatrix);
	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0, 0, 0, 0));

	if (map->getRsw()->water.type.value >= 0)
	{
		float opacity = map->getRsw()->water.type != 4 && map->getRsw()->water.type != 6 ? 0.6f : 1.0f;
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(1, 1, 1, opacity));
		highlightRenderState.activeTexture[0] = waterTextures[(int)(blib::util::Profiler::getAppTime() * map->getRsw()->water.animSpeed) % waterTextures.size()];

		{
			float repeatX = map->getGnd()->width / 4.0f;
			float repeatY = map->getGnd()->height / 4.0f;

			float waterHeight = map->getRsw()->water.height;

			std::vector<blib::VertexP3T2> verts;
			verts.push_back(blib::VertexP3T2(glm::vec3(0, -waterHeight, 0), glm::vec2(0, 0)));
			verts.push_back(blib::VertexP3T2(glm::vec3(0, -waterHeight, 10 * map->getGnd()->height), glm::vec2(0, repeatY)));
			verts.push_back(blib::VertexP3T2(glm::vec3(10 * map->getGnd()->width, -waterHeight, 0), glm::vec2(repeatX, 0)));

			verts.push_back(blib::VertexP3T2(glm::vec3(10 * map->getGnd()->width, -waterHeight, 10 * map->getGnd()->height), glm::vec2(repeatX, repeatY)));
			verts.push_back(blib::VertexP3T2(glm::vec3(0, -waterHeight, 10 * map->getGnd()->height), glm::vec2(0, repeatY)));
			verts.push_back(blib::VertexP3T2(glm::vec3(10 * map->getGnd()->width, -waterHeight, 0), glm::vec2(repeatX, 0)));
			renderer->drawTriangles(verts, highlightRenderState);
		}
	}

	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(1, 0, 0, 1));
	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
	highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, glm::translate(cameraMatrix, glm::vec3(map->getGnd()->width*5, 0, map->getGnd()->height*5)));
	highlightRenderState.activeTexture[0] = NULL;

	/* quadtree visualisation
	map->getRsw()->quadtree->foreachLevel([&renderer, this](Rsw::QuadTreeNode* node, int level) {
		if (level == 4)
		{
			std::vector<blib::VertexP3> verts = blib::Shapes::linebox(node->bbox.min * glm::vec3(1, -1, -1), node->bbox.max * glm::vec3(1, -1, -1));
			renderer->drawLines(verts, highlightRenderState);
		}
	});*/

	


	if (drawGat)
		renderGat(renderer);




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

	gndRenderState.activeShader = resourceManager->getResource<blib::Shader>("gnd");
	gndRenderState.activeShader->bindAttributeLocation("a_position", 0);
	gndRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	gndRenderState.activeShader->bindAttributeLocation("a_texture2", 2);
	gndRenderState.activeShader->bindAttributeLocation("a_tileColorCoord", 3);
	gndRenderState.activeShader->bindAttributeLocation("a_normal", 4);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::ModelViewMatrix, "modelViewMatrix", blib::Shader::Mat4);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::s_texture, "s_texture", blib::Shader::Int);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::s_lighting, "s_lighting", blib::Shader::Int);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::s_tileColor, "s_tileColor", blib::Shader::Int);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::lightAmbient, "lightAmbient", blib::Shader::Vec3);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::lightDiffuse, "lightDiffuse", blib::Shader::Vec3);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::lightIntensity, "lightIntensity", blib::Shader::Float);
	gndRenderState.activeShader->setUniformName(GndShaderAttributes::lightDirection, "lightDirection", blib::Shader::Vec3);

	
	gndRenderState.activeShader->finishUniformSetup();

	gndRenderState.activeShader->setUniform(GndShaderAttributes::s_texture, 0);
	gndRenderState.activeShader->setUniform(GndShaderAttributes::s_lighting, 1);
	gndRenderState.activeShader->setUniform(GndShaderAttributes::s_tileColor, 2);
	gndRenderState.activeFbo = fbo;
	gndRenderState.blendEnabled = true;
	gndRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	gndRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	gndRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	gndRenderState.depthTest = true;

	gndShadow = resourceManager->getResource<blib::Texture>(2048, 2048);
	gndNoShadow = resourceManager->getResource<blib::Texture>(1,1);
	gndTileColorWhite = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");
	gndTileColors = resourceManager->getResource<blib::Texture>(1024, 1024);
	gndTextureGridVbo = resourceManager->getResource<blib::VBO>();
	gndTextureGridVbo->setVertexFormat<blib::VertexP3>();

	gndGridVbo = resourceManager->getResource<blib::VBO>();
	gndGridVbo->setVertexFormat<blib::VertexP3>();



	rswRenderState.activeShader = resourceManager->getResource<blib::Shader>("rsw");
	rswRenderState.activeShader->bindAttributeLocation("a_position", 0);
	rswRenderState.activeShader->bindAttributeLocation("a_texture", 1);
	rswRenderState.activeShader->bindAttributeLocation("a_normal", 2);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::CameraMatrix, "cameraMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ModelMatrix, "modelMatrix", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::ModelMatrix2, "modelMatrix2", blib::Shader::Mat4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::s_texture, "s_texture", blib::Shader::Int);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::highlightColor, "highlightColor", blib::Shader::Vec4);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::billboard, "billboard", blib::Shader::Float);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::lightAmbient, "lightAmbient", blib::Shader::Vec3);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::lightDiffuse, "lightDiffuse", blib::Shader::Vec3);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::lightIntensity, "lightIntensity", blib::Shader::Float);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::lightDirection, "lightDirection", blib::Shader::Vec3);
	rswRenderState.activeShader->setUniformName(RswShaderAttributes::shadeType, "shadeType", blib::Shader::Int);
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

	highlightRenderState.activeShader = resourceManager->getResource<blib::Shader>("highlight");
	highlightRenderState.activeShader->bindAttributeLocation("a_position", 0);
	highlightRenderState.activeShader->bindAttributeLocation("a_texcoord", 1);
	highlightRenderState.activeShader->bindAttributeLocation("a_normal", 3);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::s_texture, "s_texture", blib::Shader::Int);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::color, "color", blib::Shader::Vec4);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::diffuse, "diffuse", blib::Shader::Float);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::texMult, "texMult", blib::Shader::Vec4);
	highlightRenderState.activeShader->setUniformName(HighlightShaderUniforms::normalMatrix, "normalMatrix", blib::Shader::Mat3);
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
	gndTextureGridDirty = true;
	gndGridDirty = true;
	gatDirty = true;
	gndTileColorDirty = false;



	gatVbo = resourceManager->getResource<blib::VBO>();
	gatVbo->setVertexFormat<blib::VertexP3T2>();

	gatTexture = resourceManager->getResource<blib::Texture>("assets/textures/gat.png");

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



	if (!map)
		return;

	gndChunks.resize((int)ceil(map->getGnd()->height / (float)CHUNKSIZE), std::vector<GndChunk*>((int)ceil(map->getGnd()->width / (float)CHUNKSIZE), NULL));
	for(size_t y = 0; y < gndChunks.size(); y++)
		for(size_t x = 0; x < gndChunks[y].size(); x++)
			gndChunks[y][x] = new GndChunk(x*CHUNKSIZE,y*CHUNKSIZE, resourceManager);
	gndShadowDirty = true;
	gndTextureGridDirty = true;
	gndGridDirty = true;
	gndTileColorDirty = true;

	//load textures if needed
	for (size_t i = 0; i < map->getGnd()->textures.size(); i++)
		if (map->getGnd()->textures[i]->texture == NULL)
			map->getGnd()->textures[i]->texture = resourceManager->getResource<blib::Texture>("data/texture/" + map->getGnd()->textures[i]->file);

	updateWaterTextures();

	map->getRsw()->water.type.callback = [this](const int &old, const int &newValue)
	{
		updateWaterTextures();
	};

}

#pragma region GND

void MapRenderer::renderGnd(blib::Renderer* renderer)
{
	//load textures if needed
	for (size_t i = 0; i < map->getGnd()->textures.size(); i++)
		if (map->getGnd()->textures[i]->texture == NULL)
			map->getGnd()->textures[i]->texture = resourceManager->getResource<blib::Texture>("data/texture/" + map->getGnd()->textures[i]->file);


	if (gndShadowDirty)
	{
		char shadowData[4] = { 0, 0, 0, 0xffu };
		renderer->setTextureSubImage(gndNoShadow, 0, 0, 1, 1, shadowData);

		new blib::BackgroundTask<char*>(app,
			[this, renderer] {
			char* data = new char[2048 * 2048 * 4];
			int x = 0; int y = 0;
			for (size_t i = 0; i < map->getGnd()->lightmaps.size(); i++)
			{
				Gnd::Lightmap* lightMap = map->getGnd()->lightmaps[i];
				for (int xx = 0; xx < 8; xx++)
				{
					for (int yy = 0; yy < 8; yy++)
					{
						int xxx = 8 * x + xx;
						int yyy = 8 * y + yy;

						data[4 * (xxx + 2048 * yyy) + 0] = (lightMap->data[64 + 3 * (xx + 8 * yy) + 0] >> 4) << 4;
						data[4 * (xxx + 2048 * yyy) + 1] = (lightMap->data[64 + 3 * (xx + 8 * yy) + 1] >> 4) << 4;
						data[4 * (xxx + 2048 * yyy) + 2] = (lightMap->data[64 + 3 * (xx + 8 * yy) + 2] >> 4) << 4;
						data[4 * (xxx + 2048 * yyy) + 3] = lightMap->data[xx + 8 * yy];
					}
				}
				x++;
				if (x * 8 >= 2048)
				{
					x = 0;
					y++;
					if (y * 8 >= 2048)
					{
						Log::out << "Lightmap too big!" << Log::newline;
					}
				}
			}
			return data;
		}, [renderer, this](char* data)
		{
			renderer->setTextureSubImage(gndShadow, 0, 0, 2048, 2048, data);
			delete[] data;
		});

		gndShadowDirty = false;
	}


	if (gndTileColorDirty)
	{

		/*new blib::BackgroundTask<char*>(app,
			[this, renderer] {*/
			char* data = new char[1024 * 1024 * 4];
			for (int x = 0; x < map->getGnd()->width; x++)
			{
				for (int y = 0; y < map->getGnd()->height; y++)
				{
					if (map->getGnd()->cubes[x][y]->tileUp != -1)
					{
						Gnd::Tile* tile = map->getGnd()->tiles[map->getGnd()->cubes[x][y]->tileUp];
						data[4 * (x + 1024 * y) + 0] = tile->color.r;
						data[4 * (x + 1024 * y) + 1] = tile->color.g;
						data[4 * (x + 1024 * y) + 2] = tile->color.b;
						data[4 * (x + 1024 * y) + 3] = tile->color.a;
					}
				}
			}
	//		return data;
	//	}, [renderer, this](char* data)
	//	{
			renderer->setTextureSubImage(gndTileColors, 0, 0, 1024, 1024, data);
			delete[] data;

	//	});

		gndTileColorDirty = false;
	}

	//render gnd chunks
	gndRenderState.activeShader->setUniform(GndShaderAttributes::ModelViewMatrix, cameraMatrix);


	if (drawShadows)
	{
		gndRenderState.activeTexture[1] = gndShadow;
		gndRenderState.activeTexture[2] = gndTileColors;
	}
	else
	{
		gndRenderState.activeTexture[1] = gndNoShadow;
		gndRenderState.activeTexture[2] = gndTileColorWhite;
	}
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
	rebuilding = true;

	struct NewChunkData
	{
		std::vector<GndVertex> allVerts;
		std::vector<VboIndex> newVertIndices;
	};

	for (auto a : vertIndices)
	{
		if (a.texture >= (int)gnd->textures.size())
		{
			vertIndices.clear();
			break;
		}
	}


	new blib::BackgroundTask<NewChunkData>(app, [this, gnd, renderer] () {
//		Log::out << "Rebuilding chunk " << x << ", " << y << Log::newline;
		std::map<int, std::vector<GndVertex> > verts;
		NewChunkData ret;
		for(int x = this->x; x < glm::min(this->x+CHUNKSIZE, (int)gnd->cubes.size()); x++)
		{
			for(int y = this->y; y < glm::min(this->y+CHUNKSIZE, (int)gnd->cubes[x].size()); y++)
			{
				Gnd::Cube* cube = gnd->cubes[x][y];

				if(cube->tileUp != -1)
				{
					Gnd::Tile* tile = gnd->tiles[cube->tileUp];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10*x,	-cube->h3,10*gnd->height-10*y),	tile->v3,		glm::vec2(lm1.x,lm2.y), glm::vec2(x/1024.0f,	(y+1)/1024.0f), cube->normals[2]);
					GndVertex v2(glm::vec3(10*x+10,	-cube->h4,10*gnd->height-10*y),	tile->v4,		glm::vec2(lm2.x,lm2.y), glm::vec2((x+1)/1024.0f,(y+1)/1024.0f), cube->normals[3]);
					GndVertex v3(glm::vec3(10*x,	-cube->h1,10*gnd->height-10*y+10), tile->v1,	glm::vec2(lm1.x,lm1.y), glm::vec2(x/1024.0f,	(y)/1024.0f),	cube->normals[0]);
					GndVertex v4(glm::vec3(10*x+10,	-cube->h2,10*gnd->height-10*y+10), tile->v2,	glm::vec2(lm2.x,lm1.y), glm::vec2((x+1)/1024.0f,(y)/1024.0f),	cube->normals[1]);

					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
				if(cube->tileFront != -1 && x < gnd->width-1)
				{
					Gnd::Tile* tile = gnd->tiles[cube->tileFront];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10 * x + 10, -cube->h2, 10 * gnd->height - 10 * y + 10),						tile->v2, glm::vec2(lm2.x, lm1.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(1,0,0));
					GndVertex v2(glm::vec3(10 * x + 10, -cube->h4, 10 * gnd->height - 10 * y),							tile->v1, glm::vec2(lm1.x, lm1.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(1,0,0));
					GndVertex v3(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h1,	10 * gnd->height - 10 * y + 10),	tile->v4, glm::vec2(lm2.x, lm2.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(1,0,0));
					GndVertex v4(glm::vec3(10 * x + 10, -gnd->cubes[x + 1][y]->h3,	10 * gnd->height - 10 * y),			tile->v3, glm::vec2(lm1.x, lm2.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(1,0,0));
					
					verts[tile->textureIndex].push_back(v1); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v3);
					verts[tile->textureIndex].push_back(v3); verts[tile->textureIndex].push_back(v2); verts[tile->textureIndex].push_back(v4);
				}
				if (cube->tileSide != -1 && y < gnd->height-1)
				{
					Gnd::Tile* tile = gnd->tiles[cube->tileSide];
					assert(tile->lightmapIndex >= 0);

					glm::vec2 lm1((tile->lightmapIndex%256)*(8.0f/2048.0f) + 1.0f/2048.0f, (tile->lightmapIndex/256)*(8.0f/2048.0f) + 1.0f/2048.0f);
					glm::vec2 lm2(lm1 + glm::vec2(6.0f/2048.0f, 6.0f/2048.0f));

					GndVertex v1(glm::vec3(10 * x, -cube->h3, 10 * gnd->height - 10 * y),			tile->v1, glm::vec2(lm1.x, lm1.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(0,0,1));
					GndVertex v2(glm::vec3(10 * x + 10, -cube->h4, 10 * gnd->height - 10 * y),		tile->v2, glm::vec2(lm2.x, lm1.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(0,0,1));
					GndVertex v4(glm::vec3(10*x+10, -gnd->cubes[x][y+1]->h2,10*gnd->height-10*y),	tile->v4, glm::vec2(lm2.x,lm2.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(0,0,1));
					GndVertex v3(glm::vec3(10*x,    -gnd->cubes[x][y+1]->h1,10*gnd->height-10*y),	tile->v3, glm::vec2(lm1.x,lm2.y), glm::vec2(x/1024.0f,y/1024.0f), glm::vec3(0,0,1));

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
	if (!model->model)
		return;
	if (!model->matrixCached)
	{
		model->matrixCache = glm::mat4();
		model->matrixCache = glm::scale(model->matrixCache, glm::vec3(1, 1, -1));
		model->matrixCache = glm::translate(model->matrixCache, glm::vec3(5 * map->getGnd()->width + model->position.x, -model->position.y, -10-5 * map->getGnd()->height + model->position.z));
		model->matrixCache = glm::rotate(model->matrixCache, -glm::radians(model->rotation.z), glm::vec3(0, 0, 1));
		model->matrixCache = glm::rotate(model->matrixCache, -glm::radians(model->rotation.x), glm::vec3(1, 0, 0));
		model->matrixCache = glm::rotate(model->matrixCache, glm::radians(model->rotation.y), glm::vec3(0, 1, 0));
		model->matrixCache = glm::scale(model->matrixCache, glm::vec3(model->scale.x, -model->scale.y, model->scale.z));
		model->matrixCache = glm::translate(model->matrixCache, glm::vec3(-model->model->realbbrange.x, model->model->realbbmin.y, -model->model->realbbrange.z));
		model->matrixCached = true;


		std::vector<blib::VertexP3> verts = blib::Shapes::box(model->model->realbbmin, model->model->realbbmax);
		for (size_t i = 0; i < verts.size(); i++)
			verts[i].position = glm::vec3(model->matrixCache * glm::vec4(glm::vec3(1,-1,1) * verts[i].position,1.0f));
		model->aabb.min = glm::vec3(99999999, 99999999, 99999999);
		model->aabb.max = glm::vec3(-99999999, -99999999, -99999999);
		for (size_t i = 0; i < verts.size(); i++)
		{
			model->aabb.min = glm::min(model->aabb.min, verts[i].position);
			model->aabb.max = glm::max(model->aabb.max, verts[i].position);
		}
	}

	if (model->model->renderer == NULL)
	{
		model->model->renderer = new RsmModelRenderInfo();
		for (size_t i = 0; i < model->model->textures.size(); i++)
			model->model->renderer->textures.push_back(resourceManager->getResource<blib::Texture>("data/texture/" + model->model->textures[i]));
	}

	rswRenderState.activeShader->setUniform(RswShaderAttributes::shadeType, model->model->shadeType);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix2, model->matrixCache);
	renderMesh(model->model->rootMesh, glm::mat4(), model->model->renderer, renderer);
}

void MapRenderer::renderMesh(Rsm::Mesh* mesh, const glm::mat4 &matrix, RsmModelRenderInfo* renderInfo, blib::Renderer* renderer)
{
	if (mesh->renderer == NULL)
	{
		mesh->renderer = new RsmMeshRenderInfo();

		std::map<int, std::vector<blib::VertexP3T2N3> > verts;
		for (size_t i = 0; i < mesh->faces.size(); i++)
		{
			glm::vec3 normal = glm::normalize(glm::cross(mesh->vertices[mesh->faces[i]->vertices[1]] - mesh->vertices[mesh->faces[i]->vertices[0]],	mesh->vertices[mesh->faces[i]->vertices[2]] - mesh->vertices[mesh->faces[i]->vertices[0]]));
			for (int ii = 0; ii < 3; ii++)
				verts[mesh->faces[i]->texIndex].push_back(blib::VertexP3T2N3(mesh->vertices[mesh->faces[i]->vertices[ii]], mesh->texCoords[mesh->faces[i]->texvertices[ii]], normal));
		}

		std::vector<blib::VertexP3T2N3> allVerts;
		for (std::map<int, std::vector<blib::VertexP3T2N3> >::iterator it2 = verts.begin(); it2 != verts.end(); it2++)
		{
			mesh->renderer->indices.push_back(VboIndex(it2->first, allVerts.size(), it2->second.size()));
			allVerts.insert(allVerts.end(), it2->second.begin(), it2->second.end());
		}
		if (!allVerts.empty())
		{
			mesh->renderer->vbo = resourceManager->getResource<blib::VBO>();
			mesh->renderer->vbo->setVertexFormat<blib::VertexP3T2N3>();
			renderer->setVbo(mesh->renderer->vbo, allVerts);
		}
		mesh->renderer->matrix = matrix * mesh->matrix1 * mesh->matrix2;
		mesh->renderer->matrixSub = matrix * mesh->matrix1;
	}
	if (!mesh->frames.empty() || mesh->matrixDirty)
	{
		mesh->matrixDirty = false;
		mesh->calcMatrix1();
		mesh->renderer->matrix = matrix * mesh->matrix1 * mesh->matrix2;
		mesh->renderer->matrixSub = matrix * mesh->matrix1;
	}

	RsmMeshRenderInfo* meshInfo = mesh->renderer;
	if (meshInfo->vbo != nullptr)
	{
		rswRenderState.activeVbo = meshInfo->vbo;
		rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix, meshInfo->matrix);

		for (VboIndex& it : meshInfo->indices)
		{
			rswRenderState.activeTexture[0] = renderInfo->textures[mesh->textures[it.texture]];
			renderer->drawTriangles<blib::VertexP3T2N3>(it.begin, it.count, rswRenderState);
		}
	}

	for (size_t i = 0; i < mesh->children.size(); i++)
		renderMesh(mesh->children[i], meshInfo->matrixSub, renderInfo, renderer);


}

void MapRenderer::resizeGl(int width, int height, int offsetx, int offsety)
{
	this->width = width;
	this->height = height;
	if (fbo && (fbo->width != width || fbo->height != height))
		fbo->setSize(width, height);

	float ratio = width / (float)height;

	if (orthoDistance > 0)
		projectionMatrix = glm::ortho(-orthoDistance*ratio, orthoDistance*ratio, -orthoDistance, orthoDistance, -5000.0f, 5000.0f);
	else
		projectionMatrix = glm::perspective(fov, ratio, 5.0f, 5000.0f);
	if (gndRenderState.activeShader)
		gndRenderState.activeShader->setUniform(GndShaderAttributes::ProjectionMatrix, projectionMatrix);
	if (rswRenderState.activeShader)
		rswRenderState.activeShader->setUniform(RswShaderAttributes::ProjectionMatrix, projectionMatrix);

	billboardMatrix = glm::scale(glm::mat4(), glm::vec3(2*height/ (float)width, 2, 1));
}

void MapRenderer::setTileDirty(int xx, int yy)
{
	if (yy >= 0 && yy < map->getGnd()->height && xx >= 0 && xx < map->getGnd()->width)
		gndChunks[yy / CHUNKSIZE][xx / CHUNKSIZE]->dirty = true;
	gndTextureGridDirty = true;
	gndShadowDirty = true;
	gatDirty = true;
}

void MapRenderer::setAllDirty()
{
	for (size_t i = 0; i < gndChunks.size(); i++)
		for (size_t ii = 0; ii < gndChunks[i].size(); ii++)
			gndChunks[i][ii]->dirty = true;
	gndTextureGridDirty = true;
	gndShadowDirty = true;
	gatDirty = true;
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
			if (o->type == Rsw::Object::Type::Light && !drawLights)
				continue;
			if (o->type == Rsw::Object::Type::Effect && !drawEffects)
				continue;
			if (o->type == Rsw::Object::Type::Sound && !drawSounds)
				continue;


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
				o->matrixCache = glm::rotate(o->matrixCache, -glm::radians(o->rotation.z), glm::vec3(0, 0, 1));
				o->matrixCache = glm::rotate(o->matrixCache, -glm::radians(o->rotation.x), glm::vec3(1, 0, 0));
				o->matrixCache = glm::rotate(o->matrixCache, glm::radians(o->rotation.y), glm::vec3(0, 1, 0));
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
			rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix, o->matrixCache);
			rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix2, billboardMatrix);
			rswRenderState.activeShader->setUniform(RswShaderAttributes::billboard, 1.0f);
			rswRenderState.activeShader->setUniform(RswShaderAttributes::shadeType, 4);
			rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDiffuse, glm::vec3(1,1,1));
			if (o->type == Rsw::Object::Type::Light)
				rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDiffuse, dynamic_cast<Rsw::Light*>(o)->color);

			renderer->drawTriangles(verts, 6, rswRenderState);
			rswRenderState.activeShader->setUniform(RswShaderAttributes::billboard, 0.0f);


		}
	}
}


void MapRenderer::renderMeshFbo(Rsm* rsm, float rotation, blib::FBO* fbo, blib::Renderer* renderer)
{
	blib::FBO* oldFbo = rswRenderState.activeFbo;

	float dist = glm::max(glm::max(rsm->realbbmax.x - rsm->realbbmin.x, rsm->realbbmax.y - rsm->realbbmin.y), rsm->realbbmax.z - rsm->realbbmin.z);

	rswRenderState.activeShader->setUniform(RswShaderAttributes::ProjectionMatrix, glm::perspective(glm::radians(75.0f), 1.0f, 0.1f, 250.0f));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::CameraMatrix, glm::lookAt(rsm->realbbrange * glm::vec3(1, -1, 1) + glm::vec3(0, -dist/3, -dist), rsm->realbbrange * glm::vec3(1, -1, 1), glm::vec3(0, 1, 0)));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix, glm::mat4());
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix2, glm::scale(glm::rotate(glm::mat4(), glm::radians(rotation), glm::vec3(0, 1, 0)), glm::vec3(1, 1, 1)));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::billboard, 0.0f);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightAmbient, glm::vec3(0.7f, 0.7f, 0.7f));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDiffuse, glm::vec3(0.7f, 0.7f, 0.7f));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightIntensity, 1.0f);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::highlightColor, glm::vec3(0, 0, 0));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDirection, glm::vec3(0, 1, 1));

	rswRenderState.activeVbo = NULL;
	rswRenderState.activeFbo = fbo;
	rswRenderState.depthTest = true;


	if(fbo != nullptr)
		renderer->setViewPort(0,0, fbo->originalWidth, fbo->originalHeight);
	renderer->clear(glm::vec4(0, 0, 0, 1), blib::Renderer::Color | blib::Renderer::Depth, rswRenderState);
	renderMesh(rsm->rootMesh, glm::mat4(), rsm->renderer, renderer);
	renderer->setViewPort(0, 0, width, height);

	rswRenderState.activeFbo = oldFbo;
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ProjectionMatrix, projectionMatrix);
}


void MapRenderer::renderMesh(Rsm* rsm, const glm::mat4 &camera, blib::Renderer* renderer)
{
	blib::FBO* oldFbo = rswRenderState.activeFbo;

	rswRenderState.activeShader->setUniform(RswShaderAttributes::ProjectionMatrix, glm::perspective(glm::radians(75.0f), width / (float)height, 0.1f, 5000.0f));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::CameraMatrix, camera);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix, glm::mat4());
	rswRenderState.activeShader->setUniform(RswShaderAttributes::ModelMatrix2, glm::mat4());
	rswRenderState.activeShader->setUniform(RswShaderAttributes::billboard, 0.0f);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightAmbient, glm::vec3(0.7f, 0.7f, 0.7f));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDiffuse, glm::vec3(0.7f, 0.7f, 0.7f));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightIntensity, 1.0f);
	rswRenderState.activeShader->setUniform(RswShaderAttributes::highlightColor, glm::vec3(0, 0, 0));
	rswRenderState.activeShader->setUniform(RswShaderAttributes::lightDirection, glm::vec3(0, 1, 1));

	rswRenderState.activeVbo = NULL;
	rswRenderState.activeFbo = nullptr;
	rswRenderState.depthTest = true;


	renderMesh(rsm->rootMesh, glm::mat4(), rsm->renderer, renderer);

	rswRenderState.activeFbo = oldFbo;
	//rswRenderState.activeShader->setUniform(RswShaderAttributes::ProjectionMatrix, projectionMatrix);
}

void MapRenderer::updateWaterTextures()
{
	Log::out << "Reloading water textures, new texture " << map->getRsw()->water.type.value << Log::newline;
	for (auto t : waterTextures)
		resourceManager->dispose(t);
	waterTextures.clear();

	if (map->getRsw()->water.type.value < 0)
		return;

	for (int ii = 0; ii < 32; ii++)
	{
		char buf[128];
		sprintf(buf, "data/texture/ฟ๖ลอ/water%i%02i%s", map->getRsw()->water.type.value, ii, ".jpg");
		waterTextures.push_back(resourceManager->getResource<blib::Texture>(buf));
		waterTextures.back()->setTextureRepeat(true);
	}
}

void MapRenderer::renderGat(blib::Renderer* renderer)
{
	if (gatDirty) //TODO: background thread
	{
		std::vector<blib::VertexP3T2> verts;
		Gat* gat = map->getGat();

		float s = 0.25f;
		for (int x = 0; x < gat->width; x++)
		{
			for (int y = 0; y < gat->height; y++)
			{
				Gat::Tile* cube = gat->tiles[x][y];
				float tx = (cube->type % 4) * s;
				float ty = (cube->type / 4) * s;

				blib::VertexP3T2 v1(glm::vec3(5 * x, -cube->heights[2] + 0.1f, 5 * gat->height - 5 * y + 5), glm::vec2(tx, ty));
				blib::VertexP3T2 v2(glm::vec3(5 * x + 5, -cube->heights[3] + 0.1f, 5 * gat->height - 5 * y + 5), glm::vec2(tx + s, ty));
				blib::VertexP3T2 v3(glm::vec3(5 * x, -cube->heights[0] + 0.1f, 5 * gat->height - 5 * y + 10), glm::vec2(tx, ty + s));
				blib::VertexP3T2 v4(glm::vec3(5 * x + 5, -cube->heights[1] + 0.1f, 5 * gat->height - 5 * y + 10), glm::vec2(tx + s, ty + s));

				verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
				verts.push_back(v3); verts.push_back(v2); verts.push_back(v4);
			}
		}
		renderer->setVbo(gatVbo, verts);
		gatDirty = false;
	}

	if (gatVbo->getLength() > 0)
	{
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, cameraMatrix);
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, projectionMatrix);
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0, 0, 0, 0));
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(1, 1, 1, 1));
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
		highlightRenderState.depthTest = false;
		highlightRenderState.activeTexture[0] = gatTexture;
		highlightRenderState.activeVbo = gatVbo;
		renderer->drawTriangles<blib::VertexP3T2>(gatVbo->getLength(), highlightRenderState);
		highlightRenderState.activeTexture[0] = NULL;
		highlightRenderState.depthTest = true;

	}
}

void MapRenderer::setShadowDirty()
{
	gndShadowDirty = true;
}



template class blib::BackgroundTask<char*>;

#pragma endregion

RsmModelRenderInfo::~RsmModelRenderInfo()
{
	for (auto t : textures)
		blib::ResourceManager::getInstance().dispose(t);
	textures.clear();
}
