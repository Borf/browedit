#include "BrowEdit.h"
#include "Camera.h"
#include "windows/FileOpenWindow.h"
#include "windows/TextureWindow.h"
#include "windows/ObjectWindow.h"
#include "windows/ModelPropertiesWindow.h"

#include "actions/TextureEditAction.h"
#include "actions/ObjectEditAction.h"
#include "actions/GroupAction.h"

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
#include <blib/Util.h>
#include <blib/ResourceManager.h>
#include <blib/wm/ToggleMenuItem.h>
#include <blib/FBO.h>
#include <blib/Shapes.h>
#include <blib/util/Log.h>
#include <blib/linq.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using blib::util::Log;

#ifdef BLIB_WIN
#include "resource.h"
#endif


BrowEdit::BrowEdit(const Json::Value &config) : mouseRay(glm::vec3(0,0,0), glm::vec3(1,0,0))
{
	this->config = config;

	appSetup.width = config["resolution"][0u].asInt();
	appSetup.height = config["resolution"][1u].asInt();
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
}

void BrowEdit::init()
{
	std::list<blib::BackgroundTask<int>*> tasks;
	for (Json::ArrayIndex i = 0; i < config["data"]["grfs"].size(); i++)
		tasks.push_back(new blib::BackgroundTask<int>(NULL, [this, i]() { blib::util::FileSystem::registerHandler(new GrfFileSystemHandler(config["data"]["grfs"][i].asString())); return 0; }));
	for (blib::BackgroundTask<int>* task : tasks)
		task->waitForTermination();


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


	highlightRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/highlight");
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


	composeRenderState.activeShader = resourceManager->getResource<blib::Shader>("assets/shaders/compose");
	composeRenderState.activeShader->bindAttributeLocation("a_position", 0);
	composeRenderState.activeShader->bindAttributeLocation("a_texcoord", 1);
	composeRenderState.activeShader->setUniformName(ComposeShaderUniforms::s_texture, "s_texture", blib::Shader::Int);
	composeRenderState.activeShader->setUniformName(ComposeShaderUniforms::s_texture2, "s_texture2", blib::Shader::Int);
	composeRenderState.activeShader->setUniformName(ComposeShaderUniforms::sampleSize, "sampleSize", blib::Shader::Vec2);
	composeRenderState.activeShader->finishUniformSetup();
	composeRenderState.activeShader->setUniform(ComposeShaderUniforms::s_texture, 0);
	composeRenderState.activeShader->setUniform(ComposeShaderUniforms::s_texture2, 1);
	composeRenderState.activeShader->setUniform(ComposeShaderUniforms::sampleSize, glm::vec2(1.0f / 1024.0f, 1.0f / 1024.0f));



	textureWindow = new TextureWindow(resourceManager, this);
	textureWindow->setPosition(window->getWidth() - textureWindow->getWidth(), 10);

	objectWindow = new ObjectWindow(resourceManager, this);
	objectWindow->setPosition(window->getWidth() - objectWindow->getWidth(), 10);
	objectWindow->hide();


	rootMenu->setAction("file/open", [this](){
		new FileOpenWindow(resourceManager, this);
	});

	rootMenu->setAction("file/save", [this](){
		if (map)
			map->save(config["data"]["ropath"].asString() + "/data/" + map->getFileName());
	});

	rootMenu->linkToggle("display/objects", &mapRenderer.drawObjects);
	rootMenu->linkToggle("display/shadows", &mapRenderer.drawShadows);
	rootMenu->setAction("editmode/textureedit", std::bind(&BrowEdit::setEditMode, this, EditMode::TextureEdit));
	rootMenu->setAction("editmode/objectedit", std::bind(&BrowEdit::setEditMode, this, EditMode::ObjectEdit));


	rootMenu->setAction("objecttools/move", std::bind(&BrowEdit::setObjectEditMode, this, ObjectEditModeTool::Translate));
	rootMenu->setAction("objecttools/scale", std::bind(&BrowEdit::setObjectEditMode, this, ObjectEditModeTool::Scale));
	rootMenu->setAction("objecttools/rotate", std::bind(&BrowEdit::setObjectEditMode, this, ObjectEditModeTool::Rotate));


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

	camera->update(elapsedTime);

	mapRenderer.cameraMatrix = camera->getMatrix();
	mapRenderer.drawTextureGrid = mapRenderer.drawObjectGrid = dynamic_cast<blib::wm::ToggleMenuItem*>(rootMenu->getItem("display/grid"))->getValue(); // TODO: fix this

	if (mouseState.leftButton)
		mouseRay = mapRenderer.mouseRay;


	if(mouseState.middleButton)
	{
		if(keyState.isPressed(blib::Key::SHIFT))
		{
			camera->direction += (mouseState.x - lastMouseState.x) / 2.0f;
			camera->angle = glm::clamp(camera->angle + (mouseState.y - lastMouseState.y) / 2.0f, 0.0f, 90.0f);
		}
		else
		{
			camera->position -= glm::vec2(glm::vec4(mouseState.x - lastMouseState.x, mouseState.y - lastMouseState.y, 0, 0) * glm::rotate(glm::mat4(), -camera->direction, glm::vec3(0, 0, 1)));
			camera->targetPosition = camera->position;
		}
	}
	if (map)
	{
		if (keyState.isPressed(blib::Key::Z) && keyState.isPressed(blib::Key::CONTROL) && !lastKeyState.isPressed(blib::Key::Z))
			undo();
		if (keyState.isPressed(blib::Key::Y) && keyState.isPressed(blib::Key::CONTROL) && !lastKeyState.isPressed(blib::Key::Y))
			redo();


		///////////////////////////////////////////////TEXTURE EDIT

		if (editMode == EditMode::TextureEdit)
		{
			if (keyState.isPressed(blib::Key::R) && !lastKeyState.isPressed(blib::Key::R))
				textureRot = (textureRot + 1) % 4;
			if (keyState.isPressed(blib::Key::H) && !lastKeyState.isPressed(blib::Key::H))
				textureFlipH = !textureFlipH;
			if (keyState.isPressed(blib::Key::V) && !lastKeyState.isPressed(blib::Key::V))
				textureFlipV = !textureFlipV;
			if (keyState.isPressed(blib::Key::PLUS) && !lastKeyState.isPressed(blib::Key::PLUS))
				textureTargetSize += glm::ivec2(1, 1);
			if (keyState.isPressed(blib::Key::MINUS) && !lastKeyState.isPressed(blib::Key::MINUS))
				textureTargetSize = glm::max(glm::ivec2(1, 1), textureTargetSize + glm::ivec2(-1, -1));
			if (keyState.isPressed(blib::Key::LEFT) && !lastKeyState.isPressed(blib::Key::LEFT))
				textureTargetSize = glm::max(glm::ivec2(1, 1), textureTargetSize + glm::ivec2(-1, 0));
			if (keyState.isPressed(blib::Key::RIGHT) && !lastKeyState.isPressed(blib::Key::RIGHT))
				textureTargetSize += glm::ivec2(1, 0);
			if (keyState.isPressed(blib::Key::UP) && !lastKeyState.isPressed(blib::Key::UP))
				textureTargetSize += glm::ivec2(0, 1);
			if (keyState.isPressed(blib::Key::DOWN) && !lastKeyState.isPressed(blib::Key::DOWN))
				textureTargetSize = glm::max(glm::ivec2(1, 1), textureTargetSize + glm::ivec2(0, -1));

			int cursorX = (int)glm::floor(mapRenderer.mouse3d.x / 10);
			int cursorY = map->getGnd()->height + 1 - (int)glm::floor(mapRenderer.mouse3d.z / 10);
			int mapHeight = map->getGnd()->height;


			if (mouseState.leftButton && !lastMouseState.leftButton && !wm->inWindow(mouseState.x, mouseState.y) && textureWindow->selectedImage != -1)
			{
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


				TextureAction* action = new TextureAction();

				for (int x = 0; x < cursorWidth; x++)
				{
					for (int y = 0; y < cursorHeight; y++)
					{
						int xx = x + cursorTopLeft.x;
						int yy = y + cursorTopLeft.y;
						if (xx < 0 || yy < 0 || xx >= map->getGnd()->width || yy >= map->getGnd()->height)
							continue;

						glm::vec2 t1 = texStart + glm::vec2(x, y) * texInc;
						glm::vec2 t2 = t1 + texInc;

						Gnd::Cube* cube = map->getGnd()->cubes[xx][yy];
						Gnd::Tile tile;
						if (cube->tileUp != -1)
							tile = Gnd::Tile(*map->getGnd()->tiles[cube->tileUp]);
						else
						{
							tile = Gnd::Tile();
							tile.lightmapIndex = 0;
						}
						tile.textureIndex = textureWindow->selectedImage;
						tile.v1 = glm::vec2(rot * glm::vec4(t1.x, t1.y, 0, 1));
						tile.v2 = glm::vec2(rot * glm::vec4(t2.x, t1.y, 0, 1));
						tile.v3 = glm::vec2(rot * glm::vec4(t1.x, t2.y, 0, 1));
						tile.v4 = glm::vec2(rot * glm::vec4(t2.x, t2.y, 0, 1));
						action->addTile(xx, yy, tile);

					}
				}

				perform(action);

			}
		}
		
		//////////////////////////////////////////////OBJECT EDIT

		if (editMode == EditMode::ObjectEdit && !wm->inWindow(mouseState.x, mouseState.y))
		{
			if (newModel)
			{
				newModel->position = glm::vec3(mapRenderer.mouse3d.x - map->getGnd()->width * 5, -mapRenderer.mouse3d.y, -mapRenderer.mouse3d.z + (10 + 5 * map->getGnd()->height));;
				newModel->matrixCached = false;
				if (mouseState.leftButton && !lastMouseState.leftButton)
					newModel = NULL;
			}

			else if (mouseState.leftButton && !lastMouseState.leftButton || mouseState.rightButton && !lastMouseState.rightButton)
			{//down
				startMouseState = mouseState;
				mouse3dstart = mapRenderer.mouse3d;

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
					objectTranslateDirection = TranslatorTool::Axis::NONE;
					objectRotateDirection = RotatorTool::Axis::NONE;

					if (objectEditModeTool == ObjectEditModeTool::Translate)
						objectTranslateDirection = translatorTool.selectedAxis(mapRenderer.mouseRay, center);
					if (objectEditModeTool == ObjectEditModeTool::Rotate)
						objectRotateDirection = rotatorTool.selectedAxis(mapRenderer.mouseRay, center);
					if (objectEditModeTool == ObjectEditModeTool::Scale)
						objectScaleDirection = scaleTool.selectedAxis(mapRenderer.mouseRay, center);


					for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
						if (map->getRsw()->objects[i]->selected)
							objectEditActions.push_back(new ObjectEditAction(map->getRsw()->objects[i], i));
				}




			}
			else if (!mouseState.leftButton && lastMouseState.leftButton)
			{//left up
				if (objectTranslateDirection != TranslatorTool::Axis::NONE || objectRotateDirection != RotatorTool::Axis::NONE || objectScaleDirection != ScaleTool::Axis::NONE)
				{
					GroupAction* action = new GroupAction();
					for (ObjectEditAction* a : objectEditActions)
						action->add(a);
					perform(action);
					objectEditActions.clear();
				}
				else if (abs(startMouseState.x - lastMouseState.x) < 2 && abs(startMouseState.y - lastMouseState.y) < 2)
				{ //click
					if (!wm->inWindow(mouseState.x, mouseState.y))
					{
						for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
						{
							Rsw::Object* o = map->getRsw()->objects[i];
							glm::vec2 pos = glm::vec2(map->getGnd()->width * 5 + o->position.x, 10 + 5 * map->getGnd()->height - o->position.z);
							float dist = glm::length(pos - glm::vec2(mapRenderer.mouse3d.x, mapRenderer.mouse3d.z));
							o->selected = o->collides(mapRenderer.mouseRay);
							if (o->selected && mouseState.clickcount == 2)
							{
								if (o->type == Rsw::Object::Type::Model)
									new ModelPropertiesWindow((Rsw::Model*)o, resourceManager);
							}
						}
					}
				}
				else
				{
					for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
					{
						Rsw::Object* o = map->getRsw()->objects[i];
						glm::vec2 tl = glm::vec2(glm::min(mouse3dstart.x, mapRenderer.mouse3d.x), glm::min(mouse3dstart.z, mapRenderer.mouse3d.z));
						glm::vec2 br = glm::vec2(glm::max(mouse3dstart.x, mapRenderer.mouse3d.x), glm::max(mouse3dstart.z, mapRenderer.mouse3d.z));
						glm::vec2 pos = glm::vec2(map->getGnd()->width * 5 + o->position.x, 10 + 5 * map->getGnd()->height - o->position.z);

						o->selected = pos.x > tl.x && pos.x < br.x && pos.y > tl.y && pos.y < br.y;
					}
				}
			}
			else if (mouseState.leftButton && lastMouseState.leftButton)
			{ // dragging				
				for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
				{
					Rsw::Object* o = map->getRsw()->objects[i];
					if (o->selected)
					{
						if (objectTranslateDirection != TranslatorTool::Axis::NONE)
						{
							if (((int)objectTranslateDirection & (int)TranslatorTool::Axis::X) != 0)
								o->position.x -= lastmouse3d.x - mapRenderer.mouse3d.x;
							if (((int)objectTranslateDirection & (int)TranslatorTool::Axis::Y) != 0)
								o->position.y += (mouseState.y - lastMouseState.y) / 5.0f;
							if (((int)objectTranslateDirection & (int)TranslatorTool::Axis::Z) != 0)
								o->position.z += lastmouse3d.z - mapRenderer.mouse3d.z;
						}
						if (objectRotateDirection != RotatorTool::Axis::NONE)
						{
							if (objectRotateDirection == RotatorTool::Axis::X)
								o->rotation.x -= mouseState.x - lastMouseState.x;
							if (objectRotateDirection == RotatorTool::Axis::Y)
								o->rotation.y -= mouseState.x - lastMouseState.x;
							if (objectRotateDirection == RotatorTool::Axis::Z)
								o->rotation.z -= mouseState.x - lastMouseState.x;
						}
						if (objectScaleDirection != ScaleTool::Axis::NONE)
						{
							if (objectScaleDirection == ScaleTool::Axis::X)
								o->scale.x *= 1 - (mouseState.x - lastMouseState.x + mouseState.y - lastMouseState.y) * 0.01f;
							if (objectScaleDirection == ScaleTool::Axis::Y)
								o->scale.y *= 1 - (mouseState.x - lastMouseState.x + mouseState.y - lastMouseState.y) * 0.01f;
							if (objectScaleDirection == ScaleTool::Axis::Z)
								o->scale.z *= 1 - (mouseState.x - lastMouseState.x + mouseState.y - lastMouseState.y) * 0.01f;
							if (objectScaleDirection == ScaleTool::Axis::ALL)
								o->scale *= 1 - (mouseState.x - lastMouseState.x + mouseState.y - lastMouseState.y) * 0.01f;
						}

						((Rsw::Model*)o)->matrixCached = false;
					}
				}



			}


			if (!mouseState.rightButton && lastMouseState.rightButton)
			{
				for (size_t i = 0; i < map->getRsw()->objects.size(); i++)
					map->getRsw()->objects[i]->selected = false;
			}

	/*		if (mouseState.rightButton)
			{//right


			}*/



		}


	}
	lastmouse3d = mapRenderer.mouse3d;
	lastKeyState = keyState;
	lastMouseState = mouseState;
}

void BrowEdit::draw()
{
	renderer->clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), blib::Renderer::Color | blib::Renderer::Depth);

	spriteBatch->begin();
	spriteBatch->draw(gradientBackground, blib::math::easyMatrix(gradientBackground, blib::math::Rectangle(0,0,window->getWidth(), window->getHeight())));
	spriteBatch->end();

	if (map)
	{
		mapRenderer.render(renderer, glm::vec2(mouseState.x, mouseState.y));


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

			for (int x = 0; x < cursorWidth; x++)
			{
				for (int y = 0; y < cursorHeight; y++)
				{
					int xx = x + cursorTopLeft.x;
					int yy = y + cursorTopLeft.y;

					if (xx < 0 || yy < 0 || xx >= map->getGnd()->width || yy >= map->getGnd()->height)
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
				}
			}


			renderer->drawTriangles(verts, highlightRenderState);
		}


		if (editMode == EditMode::ObjectEdit)
		{
			if (glm::length(glm::vec3(mapRenderer.mouse3d - mouse3dstart)) > 1 && mouseState.leftButton && !wm->inWindow(mouseState.x, mouseState.y) && (
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

void BrowEdit::loadMap(std::string fileName)
{
	if(map)
		delete map;
	map = NULL;

	new blib::BackgroundTask<Map*>(this, 	[fileName] () { return new Map(fileName); }, 
							[this] (Map* param) { map = param;
										camera->position = glm::vec2(map->getGnd()->width*5, map->getGnd()->height*5);
										camera->targetPosition = camera->position;
										mapRenderer.setMap(map);
										textureWindow->updateTextures(map); //TODO: textures aren't loaded here yet!
										objectWindow->updateObjects(map);
	} );
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
	rootMenu->setEnabled("editmode/textureedit", editMode == EditMode::TextureEdit);
	rootMenu->setEnabled("editmode/objectedit", editMode == EditMode::ObjectEdit);


	textureWindow->setVisible(editMode == EditMode::TextureEdit);
	objectWindow->setVisible(editMode == EditMode::ObjectEdit);



}

void BrowEdit::setObjectEditMode(ObjectEditModeTool newMode)
{
	this->objectEditModeTool = newMode;
	rootMenu->setEnabled("objecttools/move", objectEditModeTool == ObjectEditModeTool::Translate);
	rootMenu->setEnabled("objecttools/scale", objectEditModeTool == ObjectEditModeTool::Scale);
	rootMenu->setEnabled("objecttools/rotate", objectEditModeTool == ObjectEditModeTool::Rotate);
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