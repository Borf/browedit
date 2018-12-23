#define GLM_ENABLE_EXPERIMENTAL
#include "BromEdit.h"
#include "resource.h"
#include "windows/ModelProperties.h"
#include "windows/MeshProperties.h"
#include "windows/FrameProperties.h"
#include "windows/FileOpenWindow.h"

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
#include <blib/util/Log.h>
#include <blib/wm/Menu.h>

#include <assimp/Importer.hpp>	//OO version Header!
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>


#include <glm/gtx/quaternion.hpp>

using blib::util::Log;

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

	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(config["data"]["ropath"].get<std::string>()));

	for (size_t i = 0; i < config["data"]["grfs"].size(); i++)
		tasks.push_back(new blib::BackgroundTask<int>(NULL, [this, i]() { blib::util::FileSystem::registerHandler(new GrfFileSystemHandler(config["data"]["grfs"][i].get<std::string>())); return 0; }));
	for (blib::BackgroundTask<int>* task : tasks)
		task->waitForTermination();

	mapRenderer.init(resourceManager, this);
	mapRenderer.fov = config["fov"].get<float>();

	spriteBatch->utf8 = false;


	if (model == nullptr)
	//	loadModel("data\\model\\ũ������������\\xmas_����Ʈ��.rsm");
		loadModel("data\\model\\�δ�02\\�δ�02b�߾����01.rsm");

	grid = resourceManager->getResource<blib::Texture>("assets/textures/grid.png");
	grid->setTextureRepeat(true);
	buttons = resourceManager->getResource<blib::Texture>("assets/textures/bromedit_icons.png");
	whitePixel = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");

	auto rootMenu = wm->loadMenu("assets/bromedit.json", nullptr);
	wm->setSkin("assets/skins/ro.json", resourceManager);
	wm->setMenuBar(rootMenu);
	font = wm->font;
	font->utf8 = false;
	addMouseListener(wm);
	addKeyListener(wm);

	modelProperties = new ModelProperties(resourceManager);
	modelProperties->setPosition(window->getWidth() - modelProperties->getWidth(), 550);
	if(model)
		modelProperties->selectModel(model);


	meshProperties = new MeshProperties(resourceManager);
	meshProperties->setPosition(window->getWidth() - meshProperties->getWidth(), 20);

	frameProperties = new FrameProperties(resourceManager);
	frameProperties->setPosition(window->getWidth() - meshProperties->getWidth(), 380);

	rootMenu->setAction("file/new",				std::bind(&BromEdit::menuFileNew, this));
	rootMenu->setAction("file/open", [this](){	new FileOpenWindow(resourceManager, this);	});
	rootMenu->setAction("file/save",			std::bind(&BromEdit::menuFileSave, this));
	rootMenu->setAction("file/save as",			std::bind(&BromEdit::menuFileSaveAs, this));

}

void BromEdit::update(double elapsedTime)
{
	if (keyState.isPressed(blib::Key::ESC))
		running = false;

	if (wm->hasModalWindow())
		return;

	if (mouseState.middleButton || mouseState.rightButton)
	{
		glm::vec2 diff = glm::vec2(mouseState.position - lastMouseState.position) / 100.0f;
		rotation += diff;
	}
	distance -= mouseState.scrollPosition / 10.0f;
	mouseState.scrollPosition = 0;


	float bary = window->getHeight() - 200.0f;
	if (mouseState.leftButton && !lastMouseState.leftButton)
	{
		if (mouseState.position.y > bary+40)
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

			for (auto frame : selectedMesh->frames)
				if (fabs(frame->time / 10.0 - timeSelect) < 5)
					frameProperties->selectFrame(frame);


		}
		if (mouseState.position.y > bary && mouseState.position.y < bary + 40)
		{
			int buttonIndex = (mouseState.position.x - 5) / 37;
			switch (buttonIndex)
			{
			case 0: pause(); break;
			case 1: play(); break;
			case 2: addKeyframe(); break;
			case 3: delKeyframe(); break;
			case 4: prevFrame(); break;
			case 5: nextFrame(); break;
			case 6: addMesh(); break;
			case 7: delMesh(); break;
			case 8: replaceMesh(); break;
			}
		}
	}

	if (mouseState.position.y > bary + 40 && mouseState.position.x > 100 && mouseState.leftButton)
	{
		timeSelect = mouseState.position.x - 100.0f;
		for (auto frame : selectedMesh->frames)
			if (fabs(frame->time / 10.0 - timeSelect) < 5)
				frameProperties->selectFrame(frame);
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
		glm::vec4 color(1, 1, 1, 1);
		if (blib::math::Rectangle(glm::vec2(5 + 37 * i, y + 5), 32, 32).contains(mouseState.position))
			if(mouseState.leftButton)
				color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
			else
				color = glm::vec4(0.9f, 0.9f, 1.0f, 1.0f);
		spriteBatch->drawStretchyRect(
			blib::wm::WM::getInstance()->skinTexture,
			blib::math::easyMatrix(glm::vec2(5 + 37*i, y+5)), blib::wm::WM::getInstance()->skin["button"], glm::vec2(32, 32), color);
		
		spriteBatch->draw(buttons, blib::math::easyMatrix(glm::vec2(5 + 37 * i+4, y + 5+4)), glm::vec2(0, 0), blib::math::Rectangle(0.1f * i, 0.0f, 0.1f, 1.0f));
	}


	spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, blib::math::Rectangle(100, (int)y+40, 1, 200)), glm::vec4(0,0,0,1));
	spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, blib::math::Rectangle((int)(100 + timeSelect), (int)y+ 40, 1, 200)), glm::vec4(0, 0, 0, 1));



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


		for (auto frame : mesh->frames)
		{
			std::string symbol = "o";
			if (abs(frame->time / 10 - timeSelect) < 5 && mesh == selectedMesh)
				symbol = "x";
			spriteBatch->draw(font, symbol, blib::math::easyMatrix(glm::vec2(100 + frame->time / 10, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));
		}

		if (mesh->frames.size() > 0 && mesh->frames[mesh->frames.size() - 1]->time != 0)
		{
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


void BromEdit::loadModel(const std::string &fileName)
{
	model = new Rsm(fileName);
	distance = glm::min(100.0f, glm::max(glm::max(model->realbbmax.x - model->realbbmin.x, model->realbbmax.y - model->realbbmin.y), model->realbbmax.z - model->realbbmin.z));

	renderInfo = new RsmModelRenderInfo();
	for (size_t i = 0; i < model->textures.size(); i++)
		renderInfo->textures.push_back(resourceManager->getResource<blib::Texture>("data/texture/" + model->textures[i]));

	model->renderer = renderInfo;
	selectedMesh = model->rootMesh;
	if(modelProperties)
		modelProperties->selectModel(model);
}


void BromEdit::pause()
{
}

void BromEdit::play()
{
}

void BromEdit::addKeyframe()
{
	auto frame = new Rsm::Mesh::Frame();
	frame->time = (int)(timeSelect * 10);

	frameProperties->selectFrame(frame);

	for (auto it = selectedMesh->frames.begin(); it != selectedMesh->frames.end(); it++)
	{
		if ((*it)->time / 10 > timeSelect) {
			frame->quaternion = (*it)->quaternion;
			selectedMesh->frames.insert(it, frame);
			return;
		}
	}
	selectedMesh->frames.push_back(frame);
	if (selectedMesh->frames.size() == 1)
	{
		frame = new Rsm::Mesh::Frame();
		if(selectedMesh->frames[0]->time == 0)
			frame->time = 1000;
		else
			frame->time = 0;
		selectedMesh->frames.push_back(frame);

		std::sort(selectedMesh->frames.begin(), selectedMesh->frames.end(), [](Rsm::Mesh::Frame* a, Rsm::Mesh::Frame* b)
		{
			return b->time > a->time;
		});
	}


}

void BromEdit::delKeyframe()
{
	for (auto it = selectedMesh->frames.begin(); it != selectedMesh->frames.end(); it++)
	{
		if (fabs((*it)->time / 10.0 - timeSelect) < 5)
		{
			auto frame = *it;
			auto it2 = selectedMesh->frames.erase(it);
			if (it2 == selectedMesh->frames.end())
			{
				frameProperties->selectFrame(selectedMesh->frames[0]);
				timeSelect = selectedMesh->frames[0]->time / 10.0f;
			}
			else
			{
				frameProperties->selectFrame(*it2);
				timeSelect = (*it2)->time / 10.0f;
			}
			delete frame;
			break;
		}
	}

	if (selectedMesh->frames.size() == 1)
	{
		delete selectedMesh->frames[0];
		selectedMesh->frames.clear();
	}

}

void BromEdit::prevFrame()
{
	for (std::size_t i = 0; i < selectedMesh->frames.size(); i++)
	{
		auto frame = selectedMesh->frames[i];
		if (fabs(frame->time / 10.0 - timeSelect) < 5)
		{
			frameProperties->selectFrame(selectedMesh->frames[(i + selectedMesh->frames.size() - 1) % selectedMesh->frames.size()]);
			timeSelect = selectedMesh->frames[(i + selectedMesh->frames.size() - 1) % selectedMesh->frames.size()]->time / 10.0f;
			break;
		}
	}
}

void BromEdit::nextFrame()
{
	for (std::size_t i = 0; i < selectedMesh->frames.size(); i++)
	{
		auto frame = selectedMesh->frames[i];
		if (fabs(frame->time / 10.0 - timeSelect) < 5)
		{
			frameProperties->selectFrame(selectedMesh->frames[(i+1)% selectedMesh->frames.size()]);
			timeSelect = selectedMesh->frames[(i + 1) % selectedMesh->frames.size()]->time / 10.0f;
			break;
		}
	}
}


void BromEdit::addMesh()
{

}

void BromEdit::delMesh()
{

}

void BromEdit::menuFileNew()
{

}


void BromEdit::menuFileSave()
{

}

void BromEdit::menuFileSaveAs()
{

}



void BromEdit::replaceMesh()
{
	std::string filename = "D:\\CloudStation\\Collection\\Models\\env\\_Stylized\\mini-house\\house.fbx";
	auto mesh = model->rootMesh;
	Assimp::Importer importer;

	mesh->faces.clear();
	mesh->vertices.clear();
	mesh->texCoords.clear();
	mesh->renderer = nullptr; //todo

	auto scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality);
	// If the import failed, report it
	if (!scene)
	{
		Log::out<<importer.GetErrorString()<<Log::newline;
		return;
	}

	std::function<void(const aiScene*, aiNode*, const glm::mat4&)> scanMesh;

	scanMesh = [&](const aiScene* scene, aiNode* node, const glm::mat4 &parentMatrix)
	{
		glm::mat4 matrix = parentMatrix * glm::transpose(glm::make_mat4((float*)&node->mTransformation));

		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			auto m = scene->mMeshes[node->mMeshes[i]];

			for (unsigned int ii = 0; ii < m->mNumVertices; ii++)
			{
				mesh->vertices.push_back(glm::vec3(m->mVertices[ii].x, m->mVertices[ii].y, m->mVertices[ii].z));
				mesh->texCoords.push_back(glm::vec2(m->mTextureCoords[0][ii].x, 1-m->mTextureCoords[0][ii].y));
			}
			auto material = scene->mMaterials[m->mMaterialIndex];

			for (unsigned int ii = 0; ii < m->mNumFaces; ii++)
			{
				Rsm::Mesh::Face* face = new Rsm::Mesh::Face();
				const auto &f = m->mFaces[ii];

				face->twoSide = false;
				face->texIndex = 0;
				face->smoothGroup = 0;

				assert(f.mNumIndices == 3);
				for (int iii = 0; iii < f.mNumIndices; iii++)
				{
					face->vertices[iii] = f.mIndices[iii];
					face->texvertices[iii] = f.mIndices[iii];
				}

				mesh->faces.push_back(face);
			}

		}

		for (int i  = 0; i < node->mNumChildren; i++)
			scanMesh(scene, node->mChildren[i], matrix);
	};

	scanMesh(scene, scene->mRootNode, glm::mat4());






}