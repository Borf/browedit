#include "BrowEdit.h"
#include "Camera.h"
#include "windows/FileOpenWindow.h"
#include "windows/TextureWindow.h"
#include "windows/ObjectWindow.h"
#include "windows/ModelPropertiesWindow.h"
#include "windows/MessageWindow.h"
#include "windows/HelpWindow.h"
#include "windows/NewMapWindow.h"

#include "actions/TextureEditAction.h"
#include "actions/ObjectEditAction.h"
#include "actions/GroupAction.h"
#include "actions/SelectObjectAction.h"

#include <BroLib/GrfFileSystemHandler.h>
#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>
#include <blib/BackgroundTask.h>

#include <blib/config.h>
#include <blib/wm/WM.h>
#include <blib/wm/Menu.h>
#include <blib/util/FileSystem.h>
#include <blib/Window.h>
#include <blib/Color.h>
#include <blib/Math.h>
#include <blib/math/Polygon.h>
#include <blib/Util.h>
#include <blib/ResourceManager.h>
#include <blib/wm/ToggleMenuItem.h>
#include <blib/FBO.h>
#include <blib/Shapes.h>
#include <blib/util/Log.h>
#include <blib/linq.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

using blib::util::Log;

#ifdef BLIB_WIN
#include "resource.h"
#endif


BrowEdit::BrowEdit(const blib::json::Value &config, v8::Isolate* isolate) : mouseRay(glm::vec3(0,0,0), glm::vec3(1,0,0))
{
	this->config = config;
	this->isolate = isolate;

	appSetup.window.setWidth((float)config["resolution"][0u].asInt());
	appSetup.window.setHeight((float)config["resolution"][1u].asInt());
	appSetup.vsync = false;
	appSetup.icon = 0;
	appSetup.renderer = blib::AppSetup::GlRenderer;
	appSetup.border = true;
	



	appSetup.title = "BrowEdit 2.0";

#ifdef BLIB_WIN
	appSetup.title += " Version ";

	appSetup.icon = IDI_ICON1;

	HINSTANCE hInst = GetModuleHandle(0);
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_DATA1), "data");
	HGLOBAL hMem = LoadResource(NULL, hRes);
	DWORD size = SizeofResource(NULL, hRes);
	char* resText = (char*)LockResource(hMem);
	char* text = (char*)malloc(size + 1);
	memcpy(text, resText, size);
	text[size] = 0;
	appSetup.title += text;
	free(text);
	FreeResource(hMem);

	hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_DATA2), "data");
	hMem = LoadResource(NULL, hRes);
	size = SizeofResource(NULL, hRes);
	resText = (char*)LockResource(hMem);
	text = (char*)malloc(size + 1);
	memcpy(text, resText, size);
	text[size] = 0;
	appSetup.title += ", blib version ";
	appSetup.title += text;
	free(text);
	FreeResource(hMem);


#endif



	editMode = EditMode::TextureEdit;
	objectEditModeTool = ObjectEditModeTool::Translate;
	newModel = NULL;
	objectTranslateDirection = TranslatorTool::Axis::NONE;
	objectRotateDirection = RotatorTool::Axis::NONE;
	objectScaleDirection = ScaleTool::Axis::NONE;
	objectEditActions.clear();
	selectObjectAction = NULL;

	textureTargetSize = glm::ivec2(4, 4);
	textureRot = 0;
	textureFlipH = false;
	textureFlipV = false;



	appSetup.threaded = config["threadedrendering"].asBool();
	appSetup.backgroundTasks = config["backgroundworkers"].asBool();
	appSetup.vsync = config["vsync"].asBool();

//	appSetup.threaded = false;
//	appSetup.backgroundTasks = false;


	map = NULL;
	wm = blib::wm::WM::getInstance();


}


BrowEdit::~BrowEdit(void)
{
	if (map)
		delete map;
	map = NULL;
	//	mapRenderer.dispose();
	
	delete textureWindow;
	delete objectWindow;
	delete camera;

	delete wm;
	delete rootMenu;

	blib::linq::deleteall(objectEditActions);
	blib::linq::deleteall(actions);
	blib::linq::deleteall(undone);


	wm = NULL;
	rootMenu = NULL;
}

void BrowEdit::init()
{
	std::list<blib::BackgroundTask<int>*> tasks;
	for (size_t i = 0; i < config["data"]["grfs"].size(); i++)
		tasks.push_back(new blib::BackgroundTask<int>(NULL, [this, i]() { blib::util::FileSystem::registerHandler(new GrfFileSystemHandler(config["data"]["grfs"][i].asString())); return 0; }));
	for (blib::BackgroundTask<int>* task : tasks)
		task->waitForTermination();

	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler( config["data"]["ropath"].asString() ));


	gradientBackground = resourceManager->getResource<blib::Texture>("assets/textures/gradient.png");

	//TODO: make sure registerHandle is threadsafe!, make sure the background tasks are cleaned up

	wm->setSkin("assets/skins/ro.json", resourceManager);
	wm->setRadialMenu(rootMenu = wm->loadMenu("assets/menu.json"));
	wm->setMenuBar(rootMenu);
	addMouseListener(wm);
	addKeyListener(wm);


	addMouseListener(this);
	


	mapRenderer.init(resourceManager, this);
	mapRenderer.fov = config["fov"].asFloat();
	camera = new Camera();


	highlightRenderState.activeShader = resourceManager->getResource<blib::Shader>("highlight");
	highlightRenderState.activeShader->bindAttributeLocation("a_position", 0);
	highlightRenderState.activeShader->bindAttributeLocation("a_texcoord", 1);
	highlightRenderState.activeShader->bindAttributeLocation("a_normal", 1);
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

	highlightRenderState.depthTest = true;
	highlightRenderState.blendEnabled = true;
	highlightRenderState.srcBlendColor = blib::RenderState::SRC_ALPHA;
	highlightRenderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;
	highlightRenderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	highlightRenderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;


	composeRenderState.activeShader = resourceManager->getResource<blib::Shader>("compose");
	composeRenderState.activeShader->bindAttributeLocation("a_position", 0);
	composeRenderState.activeShader->bindAttributeLocation("a_texcoord", 1);
	composeRenderState.activeShader->setUniformName(ComposeShaderUniforms::s_texture, "s_texture", blib::Shader::Int);
	composeRenderState.activeShader->setUniformName(ComposeShaderUniforms::s_texture2, "s_texture2", blib::Shader::Int);
	composeRenderState.activeShader->finishUniformSetup();
	composeRenderState.activeShader->setUniform(ComposeShaderUniforms::s_texture, 0);
	composeRenderState.activeShader->setUniform(ComposeShaderUniforms::s_texture2, 1);



	textureWindow = new TextureWindow(resourceManager, this);
	textureWindow->setPosition(window->getWidth() - textureWindow->getWidth(), 10);

	objectWindow = new ObjectWindow(resourceManager, this);
	objectWindow->setPosition(window->getWidth() - objectWindow->getWidth(), 10);
	objectWindow->hide();

	loadJsPlugins();

	rootMenu->setAction("file/new", [this]()
	{
		new NewMapWindow(resourceManager, this);
	});

	rootMenu->setAction("file/open", [this](){
		new FileOpenWindow(resourceManager, this);
	});

	rootMenu->setAction("file/save", [this](){
		MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
		new blib::BackgroundTask<bool>(this, [this]()
		{
			if (map)
				map->save(config["data"]["ropath"].asString() + "/" + map->getFileName());
			return true;
		}, [dialog](bool bla)
		{
			dialog->close();
		});
	});

	rootMenu->setAction("file/save heightmap", [this](){
		MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
		new blib::BackgroundTask<bool>(this, [this]()
		{
			if (map)
				map->saveHeightmap(config["data"]["ropath"].asString() + "/" + map->getFileName() + ".height.png");
			return true;
		}, [dialog](bool bla)	{	dialog->close();	});
	});

	rootMenu->setAction("file/load heightmap", [this](){
		MessageWindow* dialog = new MessageWindow(resourceManager, "Saving...", "Saving");
		new blib::BackgroundTask<bool>(this, [this]()
		{
			if (map)
				map->loadHeightmap(config["data"]["ropath"].asString() + "/" + map->getFileName() + ".height.png");
			return true;
		}, [dialog](bool bla)	{	dialog->close();	});
	});


	rootMenu->setAction("display/help", [this]() { new HelpWindow(resourceManager, this);  });
	rootMenu->linkToggle("display/objects", &mapRenderer.drawObjects);
	rootMenu->linkToggle("display/shadows", &mapRenderer.drawShadows);
	rootMenu->linkToggle("display/lights", &mapRenderer.drawLights);
	rootMenu->linkToggle("display/sounds", &mapRenderer.drawSounds);
	rootMenu->linkToggle("display/effects", &mapRenderer.drawEffects);
	rootMenu->setAction("editmode/textureedit", std::bind(&BrowEdit::setEditMode, this, EditMode::TextureEdit));
	rootMenu->setAction("editmode/objectedit", std::bind(&BrowEdit::setEditMode, this, EditMode::ObjectEdit));
	rootMenu->setAction("editmode/heightedit", std::bind(&BrowEdit::setEditMode, this, EditMode::HeightEdit));
	rootMenu->setAction("editmode/detail heightedit", std::bind(&BrowEdit::setEditMode, this, EditMode::DetailHeightEdit));
	rootMenu->setAction("editmode/walledit", std::bind(&BrowEdit::setEditMode, this, EditMode::WallEdit));

	
	rootMenu->setAction("objecttools/move", std::bind(&BrowEdit::setObjectEditMode, this, ObjectEditModeTool::Translate));
	rootMenu->setAction("objecttools/scale", std::bind(&BrowEdit::setObjectEditMode, this, ObjectEditModeTool::Scale));
	rootMenu->setAction("objecttools/rotate", std::bind(&BrowEdit::setObjectEditMode, this, ObjectEditModeTool::Rotate));


	setEditMode(EditMode::TextureEdit);

//	loadMap("data/c_tower1");
#ifdef WIN32
	loadMap("data/" + config["defaultmap"].asString());
#else
	loadMap("data/prontera");
#endif
	//	loadMap("data/yuno");


}


void BrowEdit::update( double elapsedTime )
{
	if(keyState.isPressed(blib::Key::ESC))
		running = false;
	if (keyState.isPressed(blib::Key::ALT))
	{
		mouseState.middleButton = mouseState.rightButton;
		lastMouseState.middleButton = lastMouseState.rightButton;
	}


	camera->update(elapsedTime);

	mapRenderer.cameraMatrix = camera->getMatrix();
	mapRenderer.orthoDistance = camera->ortho ? camera->distance : 0;
	mapRenderer.drawTextureGrid = dynamic_cast<blib::wm::ToggleMenuItem*>(rootMenu->getItem("display/grid"))->getValue() && editMode == EditMode::TextureEdit; // TODO: fix this
	mapRenderer.drawObjectGrid = dynamic_cast<blib::wm::ToggleMenuItem*>(rootMenu->getItem("display/grid"))->getValue() && (editMode == EditMode::ObjectEdit || editMode == EditMode::HeightEdit || editMode == EditMode::DetailHeightEdit || editMode == EditMode::WallEdit); // TODO: fix this

	if (mouseState.leftButton)
		mouseRay = mapRenderer.mouseRay;



	if (!runNext.empty())
	{
		for (auto it : runNext)
		{
			it->counter--;
			if (it->counter <= 0)
				it->run();
		}
		blib::linq::deletewhere(runNext, [](JsRunner* r) { return r->counter <= 0; });
	}


	if(mouseState.middleButton)
	{
		if(keyState.isPressed(blib::Key::SHIFT))
		{
			if (mouseState.clickcount == 2)
				camera->angle = 60;
			camera->direction += (mouseState.position.x - lastMouseState.position.x) / 2.0f;
			camera->angle = glm::clamp(camera->angle + (mouseState.position.y - lastMouseState.position.y) / 2.0f, 0.0f, 90.0f);
		}
		else
		{
			if (mouseState.clickcount == 2)
				camera->direction = 0;
			camera->position -= glm::vec2(glm::vec4(mouseState.position.x - lastMouseState.position.x, mouseState.position.y - lastMouseState.position.y, 0, 0) * glm::rotate(glm::mat4(), -camera->direction, glm::vec3(0, 0, 1)));
			camera->targetPosition = camera->position;
		}
	}
	if (map)
	{
		if (keyState.isPressed(blib::Key::Z) && keyState.isPressed(blib::Key::CONTROL) && !lastKeyState.isPressed(blib::Key::Z))
			undo();
		if (keyState.isPressed(blib::Key::Y) && keyState.isPressed(blib::Key::CONTROL) && !lastKeyState.isPressed(blib::Key::Y))
			redo();


/*olroxmirror		if (keyState.isPressed(blib::Key::M) && !lastKeyState.isPressed(blib::Key::M))
		{
			int size = map->getRsw()->objects.size();
			for (int i = 0; i < size; i++)
			{
				if (map->getRsw()->objects[i]->type == Rsw::Object::Type::Model)
				{
					Rsw::Model* model = (Rsw::Model*)map->getRsw()->objects[i];
					Rsw::Model* newModel = new Rsw::Model();
					newModel->matrixCached = false;
					newModel->name = model->name;
					newModel->animType = model->animType;
					newModel->animSpeed = model->animSpeed;
					newModel->blockType = model->blockType;
					newModel->fileName = model->fileName;
					newModel->position = model->position;
					newModel->rotation = model->rotation;
					newModel->scale = model->scale;
					newModel->rotation.y = -newModel->rotation.y;
					newModel->position *= glm::vec3(-1, 1, 1);
					newModel->model = map->getRsw()->getRsm(model->fileName);
					map->getRsw()->objects.push_back(newModel);
				}
			}
		}*/

		if (!map->heightImportCubes.empty())
		{
			if (mouseState.rightButton)
			{
				if (keyState.isPressed(blib::Key::SHIFT))
					map->heightImportMax -= (mouseState.position.y - lastMouseState.position.y) * 0.1f;
				else
					map->heightImportMin -= (mouseState.position.y - lastMouseState.position.y) * 0.1f;
			}

			if (keyState.isPressed(blib::Key::ENTER))
			{
				for (int x = 0; x < map->getGnd()->width; x++)
				{
					for (int y = 0; y < map->getGnd()->height; y++)
					{
						for (int i = 0; i < 4; i++)
							map->getGnd()->cubes[x][y]->height[i] = -(-map->heightImportCubes[x][y].height[i] * (map->heightImportMax - map->heightImportMin) + map->heightImportMin);
						mapRenderer.setTileDirty(x, y);
					}
				}
				map->heightImportCubes.clear();
			}
		}
		else if (editMode == EditMode::TextureEdit)
			textureEditUpdate();
		else if (editMode == EditMode::ObjectEdit)
			objectEditUpdate();
		else if (editMode == EditMode::HeightEdit)
			heightEditUpdate();
		else if (editMode == EditMode::DetailHeightEdit)
			detailHeightEditUpdate();
		else if (editMode == EditMode::WallEdit)
			wallEditUpdate();
	}
	lastmouse3d = mapRenderer.mouse3d;
	lastKeyState = keyState;
	lastMouseState = mouseState;

	wm->keyPressed = false;

	if (!running)
		isolate->Exit();
}

void BrowEdit::draw()
{
	if (map)
	{
		mapRenderer.render(renderer, glm::vec2(mouseState.position));
	}
	renderer->clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), blib::Renderer::Color | blib::Renderer::Depth);

	spriteBatch->begin();
	spriteBatch->draw(gradientBackground, blib::math::easyMatrix(gradientBackground, blib::math::Rectangle(0,0,window->getWidth(), window->getHeight())));
	spriteBatch->end();



	if (map)
	{
		//depthinfo will be gone after rendering the fbo, so render things that need depthtesting to the fbo here
		composeRenderState.activeTexture[0] = mapRenderer.fbo;

		static blib::VertexP2T2 verts[6] = 
		{
			blib::VertexP2T2(glm::vec2(-1, -1),glm::vec2(0, 0)),
			blib::VertexP2T2(glm::vec2(1, -1), glm::vec2(1, 0)),
			blib::VertexP2T2(glm::vec2(-1, 1), glm::vec2(0, 1)),

			blib::VertexP2T2(glm::vec2(1, 1),  glm::vec2(1, 1)),
			blib::VertexP2T2(glm::vec2(1, -1), glm::vec2(1, 0)),
			blib::VertexP2T2(glm::vec2(-1, 1), glm::vec2(0, 1)),
		};

		renderer->drawTriangles(verts, 6, composeRenderState);

		int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
		int cursorY = map->getGnd()->height - (int)glm::floor(mapRenderer.mouse3d.z / 10);
		int mapHeight = map->getGnd()->height;

		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, camera->getMatrix());
		highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, mapRenderer.projectionMatrix);





		if (editMode == EditMode::TextureEdit && mapRenderer.mouse3d.w < 1 && map && cursorX >= 0 && cursorX < map->getGnd()->width && cursorY >= 0 && cursorY < mapHeight && textureWindow->selectedImage != -1)
		{
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0, 0, 0, 0));
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(1, 1, 1, 0.75f));
			highlightRenderState.activeTexture[0] = map->getGnd()->textures[textureWindow->selectedImage]->texture;
			std::vector<blib::VertexP3T2> verts;

			int cursorWidth = textureTargetSize.x;
			int cursorHeight = textureTargetSize.y;

			glm::ivec2 cursorTopLeft = glm::ivec2(cursorX, cursorY) - textureTargetSize / 2;

			glm::vec2 texStart = glm::vec2(textureWindow->tx1.x, textureWindow->tx2.y);
			glm::vec2 texInc = (textureWindow->tx2 - textureWindow->tx1) / glm::vec2(cursorWidth, -cursorHeight);
			glm::vec2 texCenter = (textureWindow->tx1 + textureWindow->tx2) / 2.0f;

			glm::mat4 rot;
			rot = glm::translate(rot, glm::vec3(texCenter, 0));
			rot = glm::rotate(rot, 90.0f * textureRot, glm::vec3(0, 0, 1));
			rot = glm::scale(rot, glm::vec3(textureFlipH ? -1 : 1, textureFlipV ? -1 : 1, 1));
			rot = glm::translate(rot, glm::vec3(-texCenter, 0));

			std::vector<blib::VertexP3> vertsGrid;


			for (int x = 0; x < cursorWidth; x++)
			{
				for (int y = 0; y < cursorHeight; y++)
				{
					int xx = x + cursorTopLeft.x;
					int yy = y + cursorTopLeft.y;

					if (xx < 0 || yy+1 < 0 || xx >= map->getGnd()->width || yy+1 >= map->getGnd()->height)
						continue;

					glm::vec2 t1 = texStart + glm::vec2(x, y) * texInc;
					glm::vec2 t2 = t1 + texInc;

					Gnd::Cube* cube = map->getGnd()->cubes[xx][yy+1];

					verts.push_back(blib::VertexP3T2(glm::vec3(10 * xx,			-cube->h1 + 0.1f, 10 * (mapHeight - yy)),		glm::vec2(rot * glm::vec4(t1.x,t1.y,0,1))));
					verts.push_back(blib::VertexP3T2(glm::vec3(10 * xx + 10,	-cube->h4 + 0.1f, 10 * (mapHeight - yy)-10),	glm::vec2(rot * glm::vec4(t2.x,t2.y,0,1))));
					verts.push_back(blib::VertexP3T2(glm::vec3(10 * xx + 10,	-cube->h2 + 0.1f, 10 * (mapHeight - yy)),		glm::vec2(rot * glm::vec4(t2.x,t1.y,0,1))));

					verts.push_back(blib::VertexP3T2(glm::vec3(10 * xx,			-cube->h1 + 0.1f, 10 * (mapHeight - yy)),		glm::vec2(rot * glm::vec4(t1.x,t1.y,0,1))));
					verts.push_back(blib::VertexP3T2(glm::vec3(10 * xx + 10,	-cube->h4 + 0.1f, 10 * (mapHeight - yy)-10),	glm::vec2(rot * glm::vec4(t2.x,t2.y,0,1))));
					verts.push_back(blib::VertexP3T2(glm::vec3(10 * xx,			-cube->h3 + 0.1f, 10 * (mapHeight - yy)-10),	glm::vec2(rot * glm::vec4(t1.x,t2.y,0,1))));


					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx,		-cube->h1 + 0.15f, 10 * (mapHeight - yy))));
					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx + 10,	-cube->h2 + 0.15f, 10 * (mapHeight - yy))));
					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx + 10,	-cube->h2 + 0.15f, 10 * (mapHeight - yy))));
					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx + 10,	-cube->h4 + 0.15f, 10 * (mapHeight - yy)-10)));
					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx + 10,	-cube->h4 + 0.15f, 10 * (mapHeight - yy)-10)));
					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx,		-cube->h3 + 0.15f, 10 * (mapHeight - yy)-10)));
					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx,		-cube->h3 + 0.15f, 10 * (mapHeight - yy)-10)));
					vertsGrid.push_back(blib::VertexP3(glm::vec3(10 * xx,		-cube->h1 + 0.15f, 10 * (mapHeight - yy))));
				}
			}
			renderer->drawTriangles(verts, highlightRenderState);

			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(1, 0, 0, 1));
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
			renderer->drawLines(vertsGrid, highlightRenderState);







		}


		if (editMode == EditMode::ObjectEdit)
		{
			if (glm::length(glm::vec3(mapRenderer.mouse3d - mouse3dstart)) > 1 && mouseState.leftButton && !wm->inWindow(mouseState.position) && (
				(objectEditModeTool == ObjectEditModeTool::Translate && objectTranslateDirection == TranslatorTool::Axis::NONE) ||
				(objectEditModeTool == ObjectEditModeTool::Rotate && objectRotateDirection == RotatorTool::Axis::NONE) ||
				(objectEditModeTool == ObjectEditModeTool::Scale && objectScaleDirection == ScaleTool::Axis::NONE)
				))
			{
				highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(1, 1, 0, 0.5f));
				highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
				highlightRenderState.activeTexture[0] = NULL;
				std::vector<blib::VertexP3> verts = blib::Shapes::box(blib::VertexP3(glm::vec3(mouse3dstart)), blib::VertexP3(glm::vec3(mapRenderer.mouse3d)));

				renderer->drawTriangles(verts, highlightRenderState);

			}

			glm::vec3 center;
			int selectCount = 0;
			for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
			{
				if (!map->getRsw()->objects[i]->selected)
					continue;
				selectCount++;
				center += glm::vec3(5 * map->getGnd()->width + map->getRsw()->objects[i]->position.x, -map->getRsw()->objects[i]->position.y, 10 + 5 * map->getGnd()->height - map->getRsw()->objects[i]->position.z);
			}

			if (selectCount > 0)
			{
				center /= selectCount;

				if (objectEditModeTool == ObjectEditModeTool::Translate)
					translatorTool.draw(mapRenderer.mouseRay, highlightRenderState, center, camera->getMatrix(), renderer);
				else if (objectEditModeTool == ObjectEditModeTool::Rotate)
					rotatorTool.draw(mapRenderer.mouseRay, highlightRenderState, center, camera->getMatrix(), renderer);
				else if (objectEditModeTool == ObjectEditModeTool::Scale)
					scaleTool.draw(mapRenderer.mouseRay, highlightRenderState, center, camera->getMatrix(), renderer);
			}

		}

		if (editMode == EditMode::HeightEdit)
		{
			std::vector<blib::VertexP3> verts;
			Gnd* gnd = map->getGnd();

			if (!selectLasso.empty())
			{
				for (size_t i = 0; i < selectLasso.size(); i++)
				{
					int x = selectLasso[i].x;
					int y = selectLasso[i].y;
					Gnd::Cube* cube = gnd->cubes[x][y];

					blib::VertexP3 v1(glm::vec3(10 * x, -cube->h3 + 0.1f, 10 * gnd->height - 10 * y));
					blib::VertexP3 v2(glm::vec3(10 * x + 10, -cube->h4 + 0.1f, 10 * gnd->height - 10 * y));
					blib::VertexP3 v3(glm::vec3(10 * x, -cube->h1 + 0.1f, 10 * gnd->height - 10 * y + 10));
					blib::VertexP3 v4(glm::vec3(10 * x + 10, -cube->h2 + 0.1f, 10 * gnd->height - 10 * y + 10));

					verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
					verts.push_back(v3); verts.push_back(v2); verts.push_back(v4);
				}
			}




			for (int x = 0; x < gnd->width; x++)
			{
				for (int y = 0; y < gnd->height; y++)
				{
					Gnd::Cube* cube = gnd->cubes[x][y];
					if (!cube->selected)
						continue;

					blib::VertexP3 v1(glm::vec3(10 * x, -cube->h3 + 0.1f, 10 * gnd->height - 10 * y));
					blib::VertexP3 v2(glm::vec3(10 * x + 10, -cube->h4 + 0.1f, 10 * gnd->height - 10 * y));
					blib::VertexP3 v3(glm::vec3(10 * x, -cube->h1 + 0.1f, 10 * gnd->height - 10 * y + 10));
					blib::VertexP3 v4(glm::vec3(10 * x + 10, -cube->h2 + 0.1f, 10 * gnd->height - 10 * y + 10));

					verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
					verts.push_back(v3); verts.push_back(v2); verts.push_back(v4);
				}
			}
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, mapRenderer.cameraMatrix);
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, mapRenderer.projectionMatrix);
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0.5f, 0.9f, 0.5f, 0.65f));
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
			highlightRenderState.activeTexture[0] = NULL;
			highlightRenderState.activeVbo = NULL;
			renderer->drawTriangles(verts, highlightRenderState);
		}

		if (editMode == EditMode::DetailHeightEdit)
		{
			std::vector<blib::VertexP3> verts;
			Gnd* gnd = map->getGnd();
			if (map->inMap(cursorX, cursorY))
			{
				if (!mouseState.leftButton)
				{
					detailHeightCursor = glm::ivec2(cursorX, cursorY);
					float cursorXOff = -(cursorX - (mapRenderer.mouse3d.x / 10));
					float cursorYOff = cursorY - (map->getGnd()->height - (mapRenderer.mouse3d.z / 10));
					detailHeightOffset = glm::vec2(cursorXOff, cursorYOff);
				}
				auto drawTriangle = [&verts, this, gnd](int x, int y, float xoff, float yoff){
					if (!map->inMap(x, y))
						return;
					Gnd::Cube* cube = gnd->cubes[x][y];
					blib::VertexP3 allVerts[4] = {
						blib::VertexP3(glm::vec3(10 * x, -cube->h3 + 0.1f, 10 * gnd->height - 10 * y)),
						blib::VertexP3(glm::vec3(10 * x + 10, -cube->h4 + 0.1f, 10 * gnd->height - 10 * y)),
						blib::VertexP3(glm::vec3(10 * x, -cube->h1 + 0.1f, 10 * gnd->height - 10 * y + 10)),
						blib::VertexP3(glm::vec3(10 * x + 10, -cube->h2 + 0.1f, 10 * gnd->height - 10 * y + 10))
					};
					int primIndex = 0;
					if (xoff < 0.5 && yoff < 0.5)
						primIndex = 0;
					else if (xoff > 0.5 && yoff < 0.5)
						primIndex = 1;
					else if (xoff < 0.5 && yoff > 0.5)
						primIndex = 2;
					else if (xoff > 0.5 && yoff > 0.5)
						primIndex = 3;

					verts.push_back(allVerts[primIndex]);
					if (xoff < 0.5)
						verts.push_back(allVerts[(primIndex + 1)%4]);
					else
						verts.push_back(allVerts[(primIndex + 3) % 4]);
					verts.push_back(allVerts[(primIndex + 2) % 4]);
				};



				drawTriangle(detailHeightCursor.x, detailHeightCursor.y, detailHeightOffset.x, detailHeightOffset.y);
				if (dynamic_cast<blib::wm::ToggleMenuItem*>(rootMenu->getItem("heighttools/connect"))->getValue())
				{
					if (detailHeightOffset.x < 0.5)
					{
						drawTriangle(detailHeightCursor.x - 1, detailHeightCursor.y, 1, detailHeightOffset.y);
						if (detailHeightOffset.y > 0.5)
							drawTriangle(detailHeightCursor.x - 1, detailHeightCursor.y - 1, 1, 0);
						else
							drawTriangle(detailHeightCursor.x - 1, detailHeightCursor.y + 1, 1, 1);
					}
					else
					{
						drawTriangle(detailHeightCursor.x + 1, detailHeightCursor.y, 0, detailHeightOffset.y);
						if (detailHeightOffset.y > 0.5)
							drawTriangle(detailHeightCursor.x + 1, detailHeightCursor.y - 1, 0, 0);
						else
							drawTriangle(detailHeightCursor.x + 1, detailHeightCursor.y + 1, 0, 1);
					}

					if (detailHeightOffset.y > 0.5)
						drawTriangle(detailHeightCursor.x, detailHeightCursor.y - 1, detailHeightOffset.x, 0);
					else
						drawTriangle(detailHeightCursor.x, detailHeightCursor.y + 1, detailHeightOffset.x, 1);
				}

				highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, mapRenderer.cameraMatrix);
				highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, mapRenderer.projectionMatrix);
				highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0.5f, 0.9f, 0.5f, 0.65f));
				highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
				highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
				highlightRenderState.activeTexture[0] = NULL;
				highlightRenderState.activeVbo = NULL;
				renderer->drawTriangles(verts, highlightRenderState);
			}
		}


		if (editMode == EditMode::WallEdit)
		{
			if (keyState.isPressed(blib::Key::SHIFT) && textureWindow->selectedImage != -1)
			{
				
				int cursorX = (int)glm::floor (mapRenderer.mouse3d.x / 10);
				int cursorY = map->getGnd()->height - (int)glm::round(mapRenderer.mouse3d.z / 10);

				float diffx = cursorX - (mapRenderer.mouse3d.x / 10.0f);
				float diffy = cursorY - (map->getGnd()->height - mapRenderer.mouse3d.z / 10.0f);

				if (diffx < -0.5f)
					diffx++;
				if (diffy > 0.5f)
					diffy--;

				bool horizontal = fabs(diffx) > fabs(diffy);

//todo: calculate extrax and extray  using a ray-plane collision, and determine the direction the normal is pointing at
				bool extrax = !horizontal && diffx > 0;
				bool extray = horizontal && diffy > 0;



				if (fabs(diffx) < 0.01 || fabs(diffy) < 0.01)
				{
					if (fabs(diffx) < fabs(diffy))
					{
						cursorX = (int)glm::round(mapRenderer.mouse3d.x / 10) - 1;
						cursorY = map->getGnd()->height - (int)glm::floor(mapRenderer.mouse3d.z / 10);
					}

					std::vector<blib::VertexP3T2> verts;
					Gnd* gnd = map->getGnd();
					int x = cursorX;
					int y = cursorY;
					int xx = 0;
					int yy = 0;

					for (int i = 0; i < newTextureSize; i++)
					{
						if (!map->inMap(x + xx, y + yy))
							continue;
						Gnd::Cube* cube = gnd->cubes[x+xx][y+yy];


						float tx1, tx2;
						float ty1, ty2;

						if (extrax || extray)
						{
							tx1 = textureWindow->tx1.x + i * ((textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize);
							tx2 = tx1 + (textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize;
							ty1 = textureWindow->tx2.y;
							ty2 = textureWindow->tx1.y;
						}
						else
						{
							tx2 = textureWindow->tx1.x + i * ((textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize);
							tx1 = tx2 + (textureWindow->tx2.x - textureWindow->tx1.x) / newTextureSize;
							ty1 = textureWindow->tx1.y;
							ty2 = textureWindow->tx2.y;
						}

						if (cube->tileFront != -1 && x < gnd->width - 1 && !horizontal)
						{
							Gnd::Tile* tile = gnd->tiles[cube->tileFront];
							assert(tile->lightmapIndex >= 0);

							blib::VertexP3T2 v1(glm::vec3(10 * (x + xx) + 10, -cube->h2, 10 * gnd->height - 10 * (y + yy) + 10),							glm::vec2(tx2, ty1));
							blib::VertexP3T2 v2(glm::vec3(10 * (x + xx) + 10, -cube->h4, 10 * gnd->height - 10 * (y + yy)),									glm::vec2(tx1, ty1));
							blib::VertexP3T2 v3(glm::vec3(10 * (x + xx) + 10, -gnd->cubes[x + xx + 1][y + yy]->h1, 10 * gnd->height - 10 * (y + yy) + 10),	glm::vec2(tx2, ty2));
							blib::VertexP3T2 v4(glm::vec3(10 * (x + xx) + 10, -gnd->cubes[x + xx + 1][y + yy]->h3, 10 * gnd->height - 10 * (y + yy)),		glm::vec2(tx1, ty2));

							verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
							verts.push_back(v3); verts.push_back(v2); verts.push_back(v4);
						}
						if (cube->tileSide != -1 && y < gnd->height - 1 && horizontal)
						{
							Gnd::Tile* tile = gnd->tiles[cube->tileSide];
							assert(tile->lightmapIndex >= 0);

							blib::VertexP3T2 v1(glm::vec3(10 * (x + xx), -cube->h3, 10 * gnd->height - 10 * (y + yy)),									glm::vec2(tx1, ty1));
							blib::VertexP3T2 v2(glm::vec3(10 * (x + xx) + 10, -cube->h4, 10 * gnd->height - 10 * (y + yy)),								glm::vec2(tx2, ty1));
							blib::VertexP3T2 v4(glm::vec3(10 * (x + xx) + 10, -gnd->cubes[x + xx][y + yy + 1]->h2, 10 * gnd->height - 10 * (y + yy)),	glm::vec2(tx2, ty2));
							blib::VertexP3T2 v3(glm::vec3(10 * (x + xx), -gnd->cubes[x + xx][y + yy + 1]->h1, 10 * gnd->height - 10 * (y + yy)),		glm::vec2(tx1, ty2));

							verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
							verts.push_back(v3); verts.push_back(v2); verts.push_back(v4);
						}

						if (horizontal)
							xx += extray ? 1 : -1;
						else
							yy += extrax ? -1 : 1;
					}

					highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, mapRenderer.cameraMatrix);
					highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, mapRenderer.projectionMatrix);
					highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
					highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(1, 1, 1, 1));
					highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
					highlightRenderState.activeTexture[0] = gnd->textures[textureWindow->selectedImage]->texture;
					highlightRenderState.activeVbo = NULL;
					renderer->drawTriangles(verts, highlightRenderState);

				}

			}
			else //no shift pressed
			{
				if (!mouseState.leftButton && !lastMouseState.leftButton)
				{
					int lastCursorX = (int)glm::round(mouse3dstart.x / 10) - 1;
					int lastCursorY = map->getGnd()->height - (int)glm::round(mouse3dstart.z / 10);
					int cursorX = (int)glm::round(mapRenderer.mouse3d.x / 10) - 1;
					int cursorY = map->getGnd()->height - (int)glm::round(mapRenderer.mouse3d.z / 10);


					if (map->inMap(cursorX, cursorY) && map->inMap(cursorX + 1, cursorY + 1))
					{
						glm::vec4 heights = map->getHeightsAt(cursorX + 1, cursorY + 1);
						float minHeight = glm::min(glm::min(glm::min(-heights.x, -heights.y), -heights.z), -heights.w);

						std::vector<blib::VertexP3> verts;
						{
							blib::VertexP3 v1(glm::vec3(10 * (cursorX + 1), minHeight, 10 * map->getGnd()->height - 10 * cursorY));
							blib::VertexP3 v2(glm::vec3(10 * (cursorX + 1), minHeight + 100, 10 * map->getGnd()->height - 10 * cursorY));

							blib::VertexP3 v3(glm::vec3(10 * (cursorX + 0.5f), minHeight, 10 * map->getGnd()->height - 10 * cursorY));
							blib::VertexP3 v4(glm::vec3(10 * (cursorX + 0.5f), minHeight + 100, 10 * map->getGnd()->height - 10 * cursorY));

							blib::VertexP3 v5(glm::vec3(10 * (cursorX + 1), minHeight, 10 * map->getGnd()->height - 10 * (cursorY - 0.5f)));
							blib::VertexP3 v6(glm::vec3(10 * (cursorX + 1), minHeight + 100, 10 * map->getGnd()->height - 10 * (cursorY - 0.5f)));

							blib::VertexP3 v7(glm::vec3(10 * (cursorX + 1), minHeight, 10 * map->getGnd()->height - 10 * (cursorY + 0.5f)));
							blib::VertexP3 v8(glm::vec3(10 * (cursorX + 1), minHeight + 100, 10 * map->getGnd()->height - 10 * (cursorY + 0.5f)));

							blib::VertexP3 v9(glm::vec3(10 * (cursorX + 1.5f), minHeight, 10 * map->getGnd()->height - 10 * cursorY));
							blib::VertexP3 v10(glm::vec3(10 * (cursorX + 1.5f), minHeight + 100, 10 * map->getGnd()->height - 10 * cursorY));

							verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
							verts.push_back(v2); verts.push_back(v3); verts.push_back(v4);

							verts.push_back(v1); verts.push_back(v2); verts.push_back(v5);
							verts.push_back(v2); verts.push_back(v5); verts.push_back(v6);

							verts.push_back(v1); verts.push_back(v2); verts.push_back(v7);
							verts.push_back(v2); verts.push_back(v7); verts.push_back(v8);

							verts.push_back(v1); verts.push_back(v2); verts.push_back(v9);
							verts.push_back(v2); verts.push_back(v9); verts.push_back(v10);

						}

						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, mapRenderer.cameraMatrix);
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, mapRenderer.projectionMatrix);
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0.5f, 0.9f, 0.5f, 0.6f));
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
						highlightRenderState.activeTexture[0] = NULL;
						highlightRenderState.activeVbo = NULL;
						renderer->drawTriangles(verts, highlightRenderState);
					}
				}


				if (mouseState.leftButton && lastMouseState.leftButton)
				{
					int lastCursorX = (int)glm::round(mouse3dstart.x / 10) - 1;
					int lastCursorY = map->getGnd()->height - (int)glm::round(mouse3dstart.z / 10);
					int cursorX = (int)glm::round(mapRenderer.mouse3d.x / 10) - 1;
					int cursorY = map->getGnd()->height - (int)glm::round(mapRenderer.mouse3d.z / 10);

					if (map->inMap(cursorX + 1, cursorY + 1) && map->inMap(lastCursorX + 1, lastCursorY + 1))
					{

						std::vector<blib::VertexP3> verts;
						if (glm::abs(lastCursorX - cursorX) < glm::abs(lastCursorY - cursorY))
							cursorX = lastCursorX;
						else
							cursorY = lastCursorY;

						glm::vec3 lastPos;
						bool first = true;

						for (int x = glm::min(cursorX, lastCursorX); x <= glm::max(cursorX, lastCursorX); x++)
						{
							for (int y = glm::min(cursorY, lastCursorY); y <= glm::max(cursorY, lastCursorY); y++)
							{
								glm::vec4 heights = map->getHeightsAt(x + 1, y + 1);
								float min = glm::min(glm::min(glm::min(-heights.x, -heights.y), -heights.z), -heights.w);
								float max = glm::max(glm::max(glm::max(-heights.x, -heights.y), -heights.z), -heights.w);


								glm::vec3 pos = glm::vec3(10 * x + 10, min, 10 * map->getGnd()->height - 10 * y);
								if (first)
								{
									lastPos = glm::vec3(10 * x + 10, min, 10 * map->getGnd()->height - 10 * y);
									first = false;
								}
								else
								{
									blib::VertexP3 v1 = lastPos;
									blib::VertexP3 v2 = lastPos + glm::vec3(0, 100, 0);
									blib::VertexP3 v3 = pos;
									blib::VertexP3 v4 = pos + glm::vec3(0, 100, 0);
									verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
									verts.push_back(v2); verts.push_back(v3); verts.push_back(v4);
									lastPos = pos;
								}
							}
						}

						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, mapRenderer.cameraMatrix);
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, mapRenderer.projectionMatrix);
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0.5f, 0.9f, 0.5f, 0.5f));
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
						highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
						highlightRenderState.activeTexture[0] = NULL;
						highlightRenderState.activeVbo = NULL;
						renderer->drawTriangles(verts, highlightRenderState);
					}
				}

			}//end no shift pressed


		}



		if (!map->heightImportCubes.empty())
		{
			std::vector<blib::VertexP3> verts;
			for (size_t x = 0; x < map->heightImportCubes.size(); x++)
			{
				for (size_t y = 0; y < map->heightImportCubes[x].size(); y++)
				{
					Gnd::Cube* cube = &map->heightImportCubes[x][y];
					blib::VertexP3 v1(glm::vec3(10 * x, -cube->h3 * (map->heightImportMax - map->heightImportMin) + map->heightImportMin, 10 * map->getGnd()->height - 10 * y));
					blib::VertexP3 v2(glm::vec3(10 * x + 10, -cube->h4 * (map->heightImportMax - map->heightImportMin) + map->heightImportMin, 10 * map->getGnd()->height - 10 * y));
					blib::VertexP3 v3(glm::vec3(10 * x, -cube->h1  * (map->heightImportMax - map->heightImportMin) + map->heightImportMin, 10 * map->getGnd()->height - 10 * y + 10));
					blib::VertexP3 v4(glm::vec3(10 * x + 10, -cube->h2 * (map->heightImportMax - map->heightImportMin) + map->heightImportMin, 10 * map->getGnd()->height - 10 * y + 10));

					verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
					verts.push_back(v3); verts.push_back(v2); verts.push_back(v4);
				}
			}
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::modelviewMatrix, mapRenderer.cameraMatrix);
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::projectionMatrix, mapRenderer.projectionMatrix);
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::color, glm::vec4(0.9f, 0.5f, 0.5f, 0.65f));
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
			highlightRenderState.activeShader->setUniform(HighlightShaderUniforms::diffuse, 0.0f);
			highlightRenderState.activeTexture[0] = NULL;
			highlightRenderState.activeVbo = NULL;
			renderer->drawTriangles(verts, highlightRenderState);
		}
		spriteBatch->begin();


		if (mapRenderer.mouse3d.w < 1 && map)
		{
			char statusText[256];
			sprintf(statusText, "Mouse: %3i, %3i,   -> %3.2f,%3.2f,%3.2f", cursorX, cursorY, mapRenderer.mouse3d.x, mapRenderer.mouse3d.y, mapRenderer.mouse3d.z);

			spriteBatch->draw(wm->font, statusText, blib::math::easyMatrix(glm::vec2(5, window->getHeight() - 18)), blib::Color::black);
			spriteBatch->draw(wm->font, statusText, blib::math::easyMatrix(glm::vec2(3, window->getHeight() - 20)), blib::Color::black);
			spriteBatch->draw(wm->font, statusText, blib::math::easyMatrix(glm::vec2(4, window->getHeight() - 19)), blib::Color::white);
		}
		std::string editModeString = "";
		if (editMode == EditMode::TextureEdit)
			editModeString = "Texture Edit";
		else if (editMode == EditMode::ObjectEdit)
			editModeString = "Object Edit";
		else if (editMode == EditMode::GatEdit)
			editModeString = "GAT Edit";
		else if (editMode == EditMode::HeightEdit)
			editModeString = "Height Edit";
		else if (editMode == EditMode::DetailHeightEdit)
			editModeString = "Detailed Height Edit";
		else if (editMode == EditMode::WallEdit)
			editModeString = "Wall Edit";
		else
			editModeString = "Unknown editmode: " + blib::util::toString((int)editMode);

		spriteBatch->draw(wm->font, editModeString, blib::math::easyMatrix(glm::vec2(301, window->getHeight() - 18)), blib::Color::black);
		spriteBatch->draw(wm->font, editModeString, blib::math::easyMatrix(glm::vec2(299, window->getHeight() - 20)), blib::Color::black);
		spriteBatch->draw(wm->font, editModeString, blib::math::easyMatrix(glm::vec2(300, window->getHeight() - 19)), blib::Color::white);
	}
	else
		spriteBatch->begin();

	wm->draw(*spriteBatch, renderer);
	spriteBatch->end();
}

void BrowEdit::loadMap(std::string fileName, bool threaded)
{
	if(map)
		delete map;
	map = NULL;
	if (threaded)
		new blib::BackgroundTask<Map*>(this, 	[fileName] () { return new Map(fileName); }, 
								[this] (Map* param) { map = param;
											camera->position = glm::vec2(map->getGnd()->width*5, map->getGnd()->height*5);
											camera->targetPosition = camera->position;
											mapRenderer.setMap(map);
											textureWindow->updateTextures(map); //TODO: textures aren't loaded here yet!
											objectWindow->updateObjects(map);
		} );
	else
	{
		map = new Map(fileName);
		camera->position = glm::vec2(map->getGnd()->width * 5, map->getGnd()->height * 5);
		camera->targetPosition = camera->position;
		mapRenderer.setMap(map);
		textureWindow->updateTextures(map); //TODO: textures aren't loaded here yet!
		objectWindow->updateObjects(map);
	}
}

bool BrowEdit::onScroll( int delta )
{
	camera->distance -= delta/10.0f;
	if (camera->distance < 0)
		camera->distance = 0;
	return true;
}



void BrowEdit::setEditMode(EditMode newMode)
{
	this->editMode = newMode;
	rootMenu->setToggleValue("editmode/textureedit", editMode == EditMode::TextureEdit);
	rootMenu->setToggleValue("editmode/objectedit", editMode == EditMode::ObjectEdit);
	rootMenu->setToggleValue("editmode/heightedit", editMode == EditMode::HeightEdit);
	rootMenu->setToggleValue("editmode/detailheight", editMode == EditMode::DetailHeightEdit);
	rootMenu->setToggleValue("editmode/walledit", editMode == EditMode::WallEdit);

	rootMenu->getItem("heighttools")->enabled = editMode == EditMode::HeightEdit;
	rootMenu->getItem("objecttools")->enabled = editMode == EditMode::ObjectEdit;

	textureWindow->setVisible(editMode == EditMode::TextureEdit || editMode == EditMode::WallEdit);
	objectWindow->setVisible(editMode == EditMode::ObjectEdit);





}

void BrowEdit::setObjectEditMode(ObjectEditModeTool newMode)
{
	this->objectEditModeTool = newMode;
	rootMenu->setToggleValue("objecttools/move", objectEditModeTool == ObjectEditModeTool::Translate);
	rootMenu->setToggleValue("objecttools/scale", objectEditModeTool == ObjectEditModeTool::Scale);
	rootMenu->setToggleValue("objecttools/rotate", objectEditModeTool == ObjectEditModeTool::Rotate);
}

void BrowEdit::addModel(const std::string &fileName)
{
	newModel = new Rsw::Model();
	newModel->matrixCached = false;
	newModel->name = fileName;
	newModel->animType = 0;
	newModel->animSpeed = 0;
	newModel->blockType = 0;
	newModel->fileName = fileName;


	newModel->position = glm::vec3(mapRenderer.mouse3d.x - map->getGnd()->width * 5, -mapRenderer.mouse3d.y, -mapRenderer.mouse3d.z + (10 + 5 * map->getGnd()->height));;
	newModel->rotation = glm::vec3(0, 0, 0);
	newModel->scale = glm::vec3(1,1,1);
	newModel->model = map->getRsw()->getRsm(newModel->fileName);
	newModel->selected = true;

	std::for_each(map->getRsw()->objects.begin(), map->getRsw()->objects.end(), [](Rsw::Object* o) { o->selected = false; });

	map->getRsw()->objects.push_back(newModel);
}

void BrowEdit::perform(Action* action)
{
	std::for_each(undone.begin(), undone.end(), [](Action* a) { delete a; });
	undone.clear();
	actions.push_back(action);
	action->perform(map, mapRenderer);
}


void BrowEdit::undo()
{
	if (!actions.empty())
	{
		Action* a = actions.back();
		actions.pop_back();
		undone.push_back(a);
		a->undo(map, mapRenderer);
	}
}

void BrowEdit::redo()
{
	if (!undone.empty())
	{
		Action* a = undone.back();
		undone.pop_back();
		actions.push_back(a);
		a->perform(map, mapRenderer);
	}
}

void JsRunner::run()
{
	v8::Handle<v8::Value> args[1] = {};
	//args[0] = browObject;

	v8::Local<v8::Function> f = v8::Local<v8::Function>::New(isolate, func);
	v8::Local<v8::Object> o = v8::Local<v8::Object>::New(isolate, obj);
	f->Call(o, 0, args);
}
