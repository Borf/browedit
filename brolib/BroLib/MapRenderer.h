#pragma once

#include <glm/glm.hpp>
#include <blib/RenderState.h>
#include <blib/gl/Vertex.h>
#include <blib/gl/GlResizeRegister.h>
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
private:
#pragma region GND
	typedef blib::VertexP3T2T2 GndVertex;
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
		};
	};	
	std::vector<std::vector<GndChunk*> > gndChunks;
	blib::Texture* gndShadow;
	blib::Texture* gndNoShadow;
	bool gndShadowDirty;
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
		};
	};

	blib::RenderState highlightRenderState;
	enum class HighlightShaderUniforms
	{
		s_texture,
		modelviewMatrix,
		projectionMatrix,
		color,
		texMult,
	};


	blib::VBO* gndTextureGridVbo;
	bool gndGridDirty;

	blib::Texture* rswLightTexture;
	blib::Texture* rswEffectTexture;
	blib::Texture* rswSoundTexture;

#pragma endregion

	blib::ResourceManager* resourceManager;

	const Map* map;
	blib::App* app;


public:
	glm::mat4 cameraMatrix;
	glm::mat4 projectionMatrix;

	bool drawShadows;
	bool drawObjects;
	bool drawLights;
	bool drawEffects;
	bool drawSounds;
	bool drawTextureGrid;
	bool drawObjectGrid;
	bool drawQuadTree;

	float fov;

	blib::FBO* fbo;
	glm::vec4 mouse3d;


	void init( blib::ResourceManager* resourceManager, blib::App* app );
	void setMap(const Map* map);

	void render(blib::Renderer* renderer, glm::vec2 mousePosition);
	void renderGnd(blib::Renderer* renderer);
	void renderRsw( blib::Renderer* renderer );
	void renderModel(Rsw::Model* model, blib::Renderer* renderer);
	void renderMesh(Rsm::Mesh* mesh, const glm::mat4 &matrix, RsmModelRenderInfo* modelInfo, blib::Renderer* renderer);

	virtual void resizeGl(int width, int height);
	void setTileDirty(int xx, int yy);
};
