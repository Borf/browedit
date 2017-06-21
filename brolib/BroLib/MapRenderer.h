#pragma once

#include <glm/glm.hpp>
#include <blib/RenderState.h>
#include <blib/gl/Vertex.h>
#include <blib/gl/GlResizeRegister.h>
#include <blib/math/Ray.h>
#include <map>
#include <vector>

#include "Rsw.h"
#include "Rsm.h"

namespace blib { class ResourceManager; class Shader; class Renderer; class App; class Texture; class VBO; };
class Map;
class Gnd;
class Rsw;
class Rsm;

#define CHUNKSIZE 16

class VboIndex
{
public:
	int texture;
	int begin;
	int count;
	VboIndex(int texture, int begin, int count)
	{
		this->texture = texture;
		this->begin = begin;
		this->count = count;
	}
};

class RsmMeshRenderInfo
{
public:
	blib::VBO* vbo;
	std::vector<VboIndex> indices;
	glm::mat4 matrix;
	glm::mat4 matrixSub;
};

class RsmModelRenderInfo
{
public:
	std::vector<blib::Texture*> textures;
};

class MapRenderer : public blib::gl::GlResizeRegister
{
public:
	typedef blib::VertexP3T2T2T2N3 GndVertex;
private:
	int width;
	int height;
#pragma region GND
	class GndChunk
	{
	public:


		bool dirty;
		bool rebuilding;
		blib::VBO* vbo;
		std::vector<VboIndex> vertIndices;

		int x, y;


		GndChunk(int x, int y, blib::ResourceManager* resourceManager);

		void render(const Gnd* gnd, blib::App* app, blib::RenderState &gndRenderState, blib::Renderer* renderer);
		void rebuild(const Gnd* gnd, blib::App* app, blib::Renderer* renderer);
	};
	//gnd
	blib::RenderState gndRenderState;
	class GndShaderAttributes
	{
	public:
		enum
		{
			ProjectionMatrix,
			ModelViewMatrix,
			s_texture,
			s_lighting,
			s_tileColor,
			lightAmbient,
			lightDiffuse,
			lightIntensity,
			lightDirection,
		};
	};	
	std::vector<std::vector<GndChunk*> > gndChunks;
	blib::Texture* gndShadow;
	blib::Texture* gndNoShadow;
	blib::Texture* gndTileColorWhite;
	blib::Texture* gndTileColors;
	bool gndShadowDirty;
	bool gndTileColorDirty;
#pragma endregion
#pragma region RSW
	blib::RenderState rswRenderState;
	class RswShaderAttributes
	{
	public:
		enum
		{
			ProjectionMatrix,
			CameraMatrix,
			ModelMatrix,
			ModelMatrix2,
			s_texture,
			highlightColor,
			billboard,
			lightAmbient,
			lightDiffuse,
			lightIntensity,
			lightDirection,
			shadeType,
		};
	};

	blib::RenderState highlightRenderState;
	enum class HighlightShaderUniforms
	{
		s_texture,
		modelviewMatrix,
		projectionMatrix,
		normalMatrix,
		diffuse,
		color,
		texMult,
	};


	blib::VBO* gndTextureGridVbo;	
	blib::VBO* gndGridVbo;


	blib::Texture* rswLightTexture;
	blib::Texture* rswEffectTexture;
	blib::Texture* rswSoundTexture;
	glm::mat4 billboardMatrix;

#pragma endregion

	blib::VBO* gatVbo;
	blib::Texture* gatTexture;

	blib::ResourceManager* resourceManager;

	const Map* map;
	blib::App* app;
	std::vector<blib::Texture*> waterTextures;


public:
	MapRenderer();
	glm::mat4 cameraMatrix;
	glm::mat4 projectionMatrix;
	float orthoDistance;

	bool drawShadows;
	bool drawObjects;
	bool drawLights;
	bool drawEffects;
	bool drawSounds;
	bool drawTextureGrid;
	bool drawObjectGrid;
	bool drawQuadTree;
	bool drawGat;

	float fov;

	blib::FBO* fbo;
	glm::vec4 mouse3d;
	blib::math::Ray mouseRay;


	void init( blib::ResourceManager* resourceManager, blib::App* app );
	void setMap(const Map* map);

	void render(blib::Renderer* renderer, glm::vec2 mousePosition);
	void renderGnd(blib::Renderer* renderer);
	void renderRsw( blib::Renderer* renderer );
	void renderGat(blib::Renderer* renderer);

	void renderObjects(blib::Renderer* renderer, bool selected);

	void renderModel(Rsw::Model* model, blib::Renderer* renderer);
	void renderMesh(Rsm::Mesh* mesh, const glm::mat4 &matrix, RsmModelRenderInfo* modelInfo, blib::Renderer* renderer);

	virtual void resizeGl(int width, int height, int offsetx, int offsety) override;
	void setTileDirty(int xx, int yy);
	void setAllDirty();
	void setShadowDirty();
	void renderMeshFbo(Rsm* rsm, float rotation, blib::FBO* fbo, blib::Renderer* renderer);
	bool gndTextureGridDirty;
	bool gndGridDirty;
	bool gatDirty;


	void updateWaterTextures();
};
