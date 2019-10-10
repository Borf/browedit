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
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <Windows.h>
#include <direct.h>

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
		//loadModel("data\\model\\prontera_re\\streetlamp_01.rsm");
	//	loadModel("data\\model\\크리스마스마을\\xmas_내부트리.rsm");
	//	loadModel("data\\model\\인던02\\인던02b중앙장식01.rsm");
		//loadModel("data\\model\\event\\3차전직_석상02.rsm"); //bigass statue
		//loadModel("data\\model\\para\\alchemy_01.rsm");
		//loadModel("data\\model\\para\\mora_01.rsm");
		//loadModel("data\\model\\para\\mora_02.rsm");
		//loadModel("data\\model\\masin\\fire_land.rsm");
		//loadModel("data\\model인던02인던02미이라.rsm");
		//loadModel("data\\model\\pud\\stall_01.rsm");
		//loadModel("data\\model\\pud\\stall_02.rsm");
		//loadModel("data\\model\\pud\\stall_03.rsm");
		//loadModel("data\\model\\pud\\swing_01.rsm");
		//loadModel("data\\model\\pud\\balloon_01.rsm");
		//loadModel("data\\model\\plants_e_01.rsm2");
		loadModel("data\\model\\ilusion\\boarding_j_01.rsm2");

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


	//testStuff();

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
			std::function<void(IRsm::IMesh*)> selectMesh;
			selectMesh = [&, this](IRsm::IMesh* mesh)
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
#ifdef FIXME
			for (auto frame : selectedRsmMesh->frames)
				if (fabs(frame->time / 10.0 - timeSelect) < 5)
					frameProperties->selectFrame(frame);
#endif

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
			case 9: exportMesh(); break;
			}
		}
	}

	if (mouseState.position.y > bary + 40 && mouseState.position.x > 100 && mouseState.leftButton)
	{
		timeSelect = mouseState.position.x - 100.0f;
		for (auto frame : selectedRsmMesh->frames)
			if (fabs(frame->time / 10.0 - timeSelect) < 5)
				frameProperties->selectFrame(frame);
		if (model->renderer->timer.isPaused())
			model->renderer->timer.setMillis(timeSelect*10);
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

	if(model->version < 0x0200)
		matrix = glm::translate(matrix, glm::vec3(model1->realbbrange));
	else
		matrix = glm::translate(matrix, glm::vec3(100));


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
	std::function<void(IRsm::IMesh*, int)> drawLine;

	drawLine = [&,this](IRsm::IMesh* mesh, int level)
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

#ifdef FIXME 
		for (auto frame : mesh->frames)
		{
			std::string symbol = "o";
			if (abs(frame->time / 10 - timeSelect) < 5 && mesh == selectedMesh)
				symbol = "x";
			spriteBatch->draw(font, symbol, blib::math::easyMatrix(glm::vec2(100 + frame->time / 10, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));
		}

		if (mesh->frames.size() > 0 && mesh->frames[mesh->frames.size() - 1]->time != 0)
		{
			int tick = model->renderer->timer.millis() % mesh->frames[mesh->frames.size() - 1]->time;
			spriteBatch->draw(font, "|", blib::math::easyMatrix(glm::vec2(100 + tick / 10, y + 40 + i * 13)), glm::vec4(0, 0, 0, 1));
		}
#endif
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
	if (fileName.substr(fileName.size() - 4) == ".rsm")
	{
		model = new Rsm(fileName);
		distance = glm::min(100.0f, glm::max(glm::max(model1->realbbmax.x - model1->realbbmin.x, model1->realbbmax.y - model1->realbbmin.y), model1->realbbmax.z - model1->realbbmin.z));
	}
	else if (fileName.substr(fileName.size() - 5) == ".rsm2")
	{
		model = new Rsm2(fileName);
	}
	mouseState.scrollPosition = 0;

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
	model->renderer->timer.pause();
}

void BromEdit::play()
{
	model->renderer->timer.resume();
}

void BromEdit::addKeyframe()
{
	auto frame = new Rsm::Mesh::Frame();
	frame->time = (int)(timeSelect * 10);

	frameProperties->selectFrame(frame);
#ifdef FIXME
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
#endif

}

void BromEdit::delKeyframe()
{
#ifdef FIXME
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
				if (model->renderer->timer.isPaused())
					model->renderer->timer.setMillis(timeSelect*10);
			}
			else
			{
				frameProperties->selectFrame(*it2);
				timeSelect = (*it2)->time / 10.0f;
				if (model->renderer->timer.isPaused())
					model->renderer->timer.setMillis(timeSelect*10);
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
#endif
}

void BromEdit::prevFrame()
{
#ifdef FIXME
	for (std::size_t i = 0; i < selectedMesh->frames.size(); i++)
	{
		auto frame = selectedMesh->frames[i];
		if (fabs(frame->time / 10.0 - timeSelect) < 5)
		{
			frameProperties->selectFrame(selectedMesh->frames[(i + selectedMesh->frames.size() - 1) % selectedMesh->frames.size()]);
			timeSelect = selectedMesh->frames[(i + selectedMesh->frames.size() - 1) % selectedMesh->frames.size()]->time / 10.0f;
			if (model->renderer->timer.isPaused())
				model->renderer->timer.setMillis(timeSelect * 10);
			break;
		}
	}
#endif
}

void BromEdit::nextFrame()
{
#ifdef FIXME
	for (std::size_t i = 0; i < selectedMesh->frames.size(); i++)
	{
		auto frame = selectedMesh->frames[i];
		if (fabs(frame->time / 10.0 - timeSelect) < 5)
		{
			frameProperties->selectFrame(selectedMesh->frames[(i+1)% selectedMesh->frames.size()]);
			timeSelect = selectedMesh->frames[(i + 1) % selectedMesh->frames.size()]->time / 10.0f;
			if (model->renderer->timer.isPaused())
				model->renderer->timer.setMillis(timeSelect * 10);
			break;
		}
	}
#endif
}


void BromEdit::addMesh()
{
#ifdef FIXME
	if (!selectedMesh)
		return;

	Rsm::Mesh* mesh = new Rsm::Mesh(model);
	mesh->name = "BorfWasHere";
	mesh->parent = selectedMesh;
	mesh->parentName = mesh->parent->name;
	mesh->parent->children.push_back(mesh);
#endif
}

void BromEdit::delMesh()
{
#ifdef FIXME
	if (!selectedMesh)
		return;
	// don't delete rootnode
	if (!selectedMesh->parent)
		return; 

	auto toDelete = selectedMesh;
	selectedMesh = selectedMesh->parent;

	selectedMesh->children.erase(std::remove(selectedMesh->children.begin(), selectedMesh->children.end(), toDelete), selectedMesh->children.end());
	delete toDelete;
#endif
}

void BromEdit::menuFileNew()
{

}


void BromEdit::menuFileSave()
{
	std::string fileName = model->fileName;
	if (fileName.find(":") != std::string::npos || fileName.find("..") != std::string::npos)
		model->save(model->fileName);
	else
		model->save(blib::util::replace(config["data"]["ropath"].get<std::string>() + "/" + fileName, "/", "\\"));

	
}

void BromEdit::menuFileSaveAs()
{
	std::string fileNameStr = model->fileName;
	if (!(fileNameStr.find(":") != std::string::npos || fileNameStr.find("..") != std::string::npos))
		fileNameStr = blib::util::replace(config["data"]["ropath"].get<std::string>() + "/" + fileNameStr, "/", "\\");

	char fileName[1024];
	strcpy_s(fileName, 1024, blib::util::replace(fileNameStr, "/", "\\").c_str());

	char curdir[100];
	_getcwd(curdir, 100);
	HWND hWnd = this->window->hWnd;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 1024;
	ofn.lpstrFilter = "All\0*.*\0RO models\0*.rsm\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn))
	{
		std::string newFileName(fileName);
		newFileName = blib::util::replace(newFileName, "/", "\\");
		if (newFileName.rfind(".") < newFileName.rfind("\\"))
			newFileName = newFileName + ".rsm";
		_chdir(curdir);

		model->save(newFileName);
	}
	_chdir(curdir);
}



void BromEdit::replaceMesh()
{
	char pFileName[1024];
	strcpy_s(pFileName, 1024, "D:\\CloudStation\\Collection\\Models\\env\\_Stylized\\mini-house\\house.fbx");

	char curdir[256];
	_getcwd(curdir, 256);
	HWND hWnd = this->window->hWnd;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pFileName;
	ofn.nMaxFile = 1024;
	ofn.lpstrFilter = "All\0*.*\0Models\0*.fbx;*.obj;*.3ds;*.lwo;*.stl|\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
	if (!GetOpenFileName(&ofn))
	{
		_chdir(curdir);
		return;
	}

	std::string filename = pFileName;
	filename = blib::util::replace(filename, "\\", "/");
	_chdir(curdir);

	auto mesh = selectedMesh ? selectedMesh : model->rootMesh;
	Assimp::Importer importer;

	mesh->faces.clear();
	mesh->vertices.clear();
	mesh->texCoords.clear();
#ifdef FIXME
	mesh->textures.clear();
	mesh->textures.push_back(0); //todo
#endif
	delete mesh->renderer;
	mesh->renderer = nullptr;

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

				face->twoSided = false;
				face->texId = 0;
				face->smoothGroup = 0;

				assert(f.mNumIndices == 3);
				for (int iii = 0; iii < f.mNumIndices; iii++)
				{
					face->vertexIds[iii] = f.mIndices[iii];
					face->texCoordIds[iii] = f.mIndices[iii];
				}

				mesh->faces.push_back(face);
			}

		}

		for (unsigned int i  = 0; i < node->mNumChildren; i++)
			scanMesh(scene, node->mChildren[i], matrix);
	};

	scanMesh(scene, scene->mRootNode, glm::mat4());






}


void BromEdit::exportMesh()
{
	if (!selectedMesh)
		return;

	char fileName[1024];
	strcpy_s(fileName, 1024, blib::util::replace(model->fileName + "." + selectedMesh->name + ".obj", "/", "\\").c_str());

	char curdir[100];
	_getcwd(curdir, 100);
	HWND hWnd = this->window->hWnd;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 1024;
	ofn.lpstrFilter = "All\0*.*\0Object files\0*.obj\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileName(&ofn))
	{
		_chdir(curdir);
		return;
	}
	std::string newFileName(fileName);
	newFileName = blib::util::replace(newFileName, "/", "\\");
	_chdir(curdir);



	aiScene scene;
	scene.mRootNode = new aiNode();
	scene.mMaterials = new aiMaterial * [1];
	scene.mNumMaterials = 1;

	scene.mMaterials[0] = new aiMaterial();

	scene.mMeshes = new aiMesh * [1];
	scene.mMeshes[0] = nullptr;
	scene.mNumMeshes = 1;

	auto mesh = scene.mMeshes[0] = new aiMesh();
	scene.mMeshes[0]->mMaterialIndex = 0;

	scene.mRootNode->mMeshes = new unsigned int[1];
	scene.mRootNode->mMeshes[0] = 0;
	scene.mRootNode->mNumMeshes = 1;

	

	std::vector<std::pair<glm::vec3, glm::vec2>> vertices;
	std::map<std::pair<int, int>, int> lookup;


	for (auto f : selectedMesh->faces)
	{
		for(int i = 0; i < 3; i++)
		{
			auto indices = std::pair<int, int>(f->vertexIds[i], f->texCoordIds[i]);
			if (lookup.find(indices) == lookup.end())
			{
				lookup[indices] = vertices.size();
				vertices.push_back(std::pair<glm::vec3, glm::vec2>(selectedMesh->vertices[indices.first], selectedMesh->texCoords[indices.second]));
			}
		}
	}

	mesh->mVertices = new aiVector3D[vertices.size()];
	mesh->mNumVertices = vertices.size();

	mesh->mTextureCoords[0] = new aiVector3D[vertices.size()];
	mesh->mNumUVComponents[0] = vertices.size();

	for (std::size_t i = 0; i < vertices.size(); i++)
	{
		mesh->mVertices[i] = aiVector3D(vertices[i].first.x, vertices[i].first.y, vertices[i].first.z);
		mesh->mTextureCoords[0][i] = aiVector3D(vertices[i].second.x, 1-vertices[i].second.y, 0);
	}

	mesh->mNumFaces = selectedMesh->faces.size();
	mesh->mFaces = new aiFace[selectedMesh->faces.size()];

	for (std::size_t i = 0; i < selectedMesh->faces.size(); i++)
	{
		auto f = selectedMesh->faces[i];
		aiFace& face = mesh->mFaces[i];
		face.mIndices = new unsigned int[3];
		face.mNumIndices = 3;

		for (int ii = 0; ii < 3; ii++)
		{
			auto indices = std::pair<int, int>(f->vertexIds[ii], f->texCoordIds[ii]);
			face.mIndices[ii] = lookup[indices];
		}
	}

	Assimp::Exporter exporter;
	exporter.Export(&scene, "obj", newFileName, aiProcess_Triangulate);

}


void BromEdit::testStuff()
{
#ifdef FIXME
	{
		auto files = blib::util::FileSystem::getFileList([](const std::string &fileName)
		{
			if (fileName.size() < 4)
				return false;
			std::string ext = fileName.substr(fileName.length() - 4, 4);
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			return ext == ".rsm";
		});

		for (const auto &f : files)
		{
			Log::out << "Loading " << f << Log::newline;
			IRsm* rsm = new IRsm(f);
			delete rsm;
		}
	}
#endif
}