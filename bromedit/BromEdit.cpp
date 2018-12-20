#define GLM_ENABLE_EXPERIMENTAL
#include "BromEdit.h"
#include "resource.h"
#include "windows/MeshProperties.h"

#include <BroLib/GrfFileSystemHandler.h>

#include <blib/wm/WM.h>
#include <blib/BackgroundTask.h>
#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/Renderer.h>
#include <blib/gl/Vertex.h>
#include <blib/Math.h>
#include <blib/Window.h>
#include <blib/util/Profiler.h>

#include <glm/gtx/quaternion.hpp>


BromEdit::BromEdit(const json &config)
{
	this->config = config;

	appSetup.window.setWidth((float)config["resolution"][0u].get<int>());
	appSetup.window.setHeight((float)config["resolution"][1u].get<int>());
	appSetup.vsync = false;
	appSetup.icon = 0;
	appSetup.renderer = blib::AppSetup::GlRenderer;
	appSetup.border = true;




	appSetup.title = "BromEdit";

#ifdef BLIB_WIN
	appSetup.title += " Version ";
	appSetup.icon = IDI_ICON1;
#endif




	appSetup.threaded = config["threadedrendering"].get<bool>();
	appSetup.backgroundTasks = config["backgroundworkers"].get<bool>();
	appSetup.vsync = config["vsync"].get<bool>();

	//	appSetup.threaded = false;
	//	appSetup.backgroundTasks = false;


	wm = blib::wm::WM::getInstance();
	model = nullptr;

}

BromEdit::~BromEdit(void)
{
}

void BromEdit::init()
{
	std::list<blib::BackgroundTask<int>*> tasks;
	for (size_t i = 0; i < config["data"]["grfs"].size(); i++)
		tasks.push_back(new blib::BackgroundTask<int>(NULL, [this, i]() { blib::util::FileSystem::registerHandler(new GrfFileSystemHandler(config["data"]["grfs"][i].get<std::string>())); return 0; }));
	for (blib::BackgroundTask<int>* task : tasks)
		task->waitForTermination();

	mapRenderer.init(resourceManager, this);
	mapRenderer.fov = config["fov"].get<float>();


	if (model == nullptr)
		model = new Rsm("data\\model\\크리스마스마을\\xmas_내부트리.rsm");
	//model = new Rsm("data\\model\\인던02\\인던02b중앙장식01.rsm");
	distance = glm::max(glm::max(model->realbbmax.x - model->realbbmin.x, model->realbbmax.y - model->realbbmin.y), model->realbbmax.z - model->realbbmin.z);

	renderInfo = new RsmModelRenderInfo();
	for (size_t i = 0; i < model->textures.size(); i++)
		renderInfo->textures.push_back(resourceManager->getResource<blib::Texture>("data/texture/" + model->textures[i]));

	model->renderer = renderInfo;

	grid = resourceManager->getResource<blib::Texture>("assets/textures/grid.png");
	grid->setTextureRepeat(true);

	wm->setSkin("assets/skins/ro.json", resourceManager);
	font = wm->font;
	addMouseListener(wm);
	addKeyListener(wm);

	meshProperties = new MeshProperties(resourceManager);
	meshProperties->setPosition(window->getWidth() - meshProperties->getWidth(), 10);

	
}

void BromEdit::update(double elapsedTime)
{
	if (keyState.isPressed(blib::Key::ESC))
		running = false;


	if (mouseState.middleButton)
	{
		glm::vec2 diff = glm::vec2(mouseState.position - lastMouseState.position) / 100.0f;
		rotation += diff;
	}
	distance -= mouseState.scrollPosition / 10.0f;
	mouseState.scrollPosition = 0;


	if (mouseState.leftButton && !lastMouseState.leftButton)
	{
		float bary = window->getHeight() - 200.0f;
		if (mouseState.position.y > bary)
		{
			int index = (int)((mouseState.position.y - bary - 40) / 13);
			std::function<void(Rsm::Mesh*)> selectMesh;
			selectMesh = [&, this](Rsm::Mesh* mesh)
			{
				if (index == 0)
				{
					selectedMesh = mesh;
					meshProperties->selectMesh(mesh);
				}
				index--;
				for (auto &m : mesh->children)
					selectMesh(m);

			};
			selectMesh(model->rootMesh);

		}
	}




	lastMouseState = mouseState;

}

void BromEdit::draw()
{
	renderer->clear(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), blib::Renderer::ClearOptions::Color | blib::Renderer::ClearOptions::Depth);

	glm::mat4 matrix;

	matrix = glm::translate(matrix, glm::vec3(0, 0, -distance));
	matrix = glm::rotate(matrix, rotation.y, glm::vec3(1, 0, 0));
	matrix = glm::rotate(matrix, rotation.x, glm::vec3(0, 1, 0));
	matrix = glm::rotate(matrix, glm::pi<float>(), glm::vec3(1, 0, 0));
	matrix = glm::translate(matrix, glm::vec3(model->realbbrange));


	renderer->setViewPort(0, 200, window->getWidth(), window->getHeight() - 200);
	mapRenderer.resizeGl(window->getWidth(), window->getHeight() - 200, 0, 0);
	mapRenderer.renderMesh(model, matrix, renderer);


	std::vector<blib::VertexP3T2> verts;
	verts.push_back(blib::VertexP3T2(glm::vec3(-100, 0, -100), glm::vec2(0, 0)));
	verts.push_back(blib::VertexP3T2(glm::vec3(100, 0, -100), glm::vec2(5, 0)));
	verts.push_back(blib::VertexP3T2(glm::vec3(100, 0, 100), glm::vec2(5, 5)));

	verts.push_back(blib::VertexP3T2(glm::vec3(100, 0, 100), glm::vec2(5, 5)));
	verts.push_back(blib::VertexP3T2(glm::vec3(-100, 0, 100), glm::vec2(0, 5)));
	verts.push_back(blib::VertexP3T2(glm::vec3(-100, 0, -100), glm::vec2(0, 0)));

	mapRenderer.rswRenderState.activeVbo = nullptr;
	mapRenderer.rswRenderState.activeShader->setUniform(MapRenderer::RswShaderAttributes::ModelMatrix, glm::mat4());
	mapRenderer.rswRenderState.activeFbo = nullptr;
	mapRenderer.rswRenderState.activeTexture[0] = grid;

	renderer->drawTriangles(verts, mapRenderer.rswRenderState);

	//end 3d viewport


	renderer->setViewPort(0, 0, window->getWidth(), window->getHeight());


	spriteBatch->begin();


	float y = window->getHeight() - 200.0f;

	spriteBatch->drawStretchyRect(
		blib::wm::WM::getInstance()->skinTexture, 
		blib::math::easyMatrix(glm::vec2(0,y)), blib::wm::WM::getInstance()->skin["miniwindow"], glm::vec2(window->getWidth(), 200));

	for (int i = 0; i < 10; i++)
	{
		spriteBatch->drawStretchyRect(
			blib::wm::WM::getInstance()->skinTexture,
			blib::math::easyMatrix(glm::vec2(5 + 37*i, y+5)), blib::wm::WM::getInstance()->skin["button"], glm::vec2(32, 32));
	}


	int i = 0;
	std::function<void(Rsm::Mesh*, int)> drawLine;

	drawLine = [&,this](Rsm::Mesh* mesh, int level)
	{
		if (mesh == selectedMesh)
		{
			spriteBatch->drawStretchyRect(blib::wm::WM::getInstance()->skinTexture, glm::translate(glm::mat4(), 
				glm::vec3(0, y + 40 + i * 13 - 1, 0)), blib::wm::WM::getInstance()->skin["list"], glm::vec2(100, 14), 
				blib::wm::WM::getInstance()->convertHexColor4(blib::wm::WM::getInstance()->skin["list"]["selectcolor"].get<std::string>()));

			spriteBatch->draw(font, mesh->name, blib::math::easyMatrix(glm::vec2(5 + 16 * level, y + 40 + i * 13)), glm::vec4(1, 1, 1, 1));
		}
		else
			spriteBatch->draw(font, mesh->name, blib::math::easyMatrix(glm::vec2(5 + 16 * level, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));

		spriteBatch->draw(font, "|", blib::math::easyMatrix(glm::vec2(100, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));

		for (auto frame : mesh->frames)
		{
			spriteBatch->draw(font, "o", blib::math::easyMatrix(glm::vec2(100 + frame->time / 10, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));
		}

		if (mesh->frames.size() > 0)
		{
			spriteBatch->draw(font, "X", blib::math::easyMatrix(glm::vec2(100 + mesh->frames[mesh->frames.size() - 1]->time / 10, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));

			int tick = (int)(blib::util::Profiler::getAppTime() * 1000) % mesh->frames[mesh->frames.size() - 1]->time;
			spriteBatch->draw(font, "|", blib::math::easyMatrix(glm::vec2(100 + tick / 10, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));
		}

		i++;
		for (auto &m : mesh->children)
			drawLine(m, level + 1);
	};

	drawLine(model->rootMesh,0);

	



	wm->draw(*spriteBatch, renderer);
	spriteBatch->end();

}
