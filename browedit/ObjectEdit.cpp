#include "BrowEdit.h"

#include <blib/wm/WM.h>

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include "actions/SelectObjectAction.h"
#include "actions/ObjectEditAction.h"
#include "actions/GroupAction.h"

#include "windows/ModelPropertiesWindow.h"
#include "windows/ObjectWindow.h"

void BrowEdit::objectEditUpdate()
{
	if (!wm->inWindow(mouseState.position))
	{
		if (newModel)
		{
			newModel->position = glm::vec3(mapRenderer.mouse3d.x - map->getGnd()->width * 5, -mapRenderer.mouse3d.y, -mapRenderer.mouse3d.z + (10 + 5 * map->getGnd()->height));;
			newModel->matrixCached = false;
			if (mouseState.leftButton && !lastMouseState.leftButton)
				newModel = NULL;
		}

		else if (mouseState.leftButton && !lastMouseState.leftButton)
		{//down
			startMouseState = mouseState;
			mouse3dstart = mapRenderer.mouse3d;

			selectObjectAction = new SelectObjectAction(map->getRsw());
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
				objectScaleDirection = ScaleTool::Axis::NONE;

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
			else if (abs(startMouseState.position.x - lastMouseState.position.x) < 2 && abs(startMouseState.position.y - lastMouseState.position.y) < 2)
			{ //click
				if (!wm->inWindow(mouseState.position))
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
								new ModelPropertiesWindow((Rsw::Model*)o, resourceManager, this);
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
			if (selectObjectAction)
			{
				selectObjectAction->finish(map->getRsw());
				if (selectObjectAction->hasDifference())
					perform(selectObjectAction);
				else
				{
					delete selectObjectAction;
					selectObjectAction = NULL;
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
							o->position.y += (mouseState.position.y - lastMouseState.position.y) / 5.0f;
						if (((int)objectTranslateDirection & (int)TranslatorTool::Axis::Z) != 0)
							o->position.z += lastmouse3d.z - mapRenderer.mouse3d.z;
					}
					if (objectRotateDirection != RotatorTool::Axis::NONE)
					{
						if (objectRotateDirection == RotatorTool::Axis::X)
							o->rotation.x -= mouseState.position.x - lastMouseState.position.x;
						if (objectRotateDirection == RotatorTool::Axis::Y)
							o->rotation.y -= mouseState.position.x - lastMouseState.position.x;
						if (objectRotateDirection == RotatorTool::Axis::Z)
							o->rotation.z -= mouseState.position.x - lastMouseState.position.x;
					}
					if (objectScaleDirection != ScaleTool::Axis::NONE)
					{
						if (objectScaleDirection == ScaleTool::Axis::X)
							o->scale.x *= 1 - (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
						if (objectScaleDirection == ScaleTool::Axis::Y)
							o->scale.y *= 1 - (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
						if (objectScaleDirection == ScaleTool::Axis::Z)
							o->scale.z *= 1 - (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
						if (objectScaleDirection == ScaleTool::Axis::ALL)
							o->scale *= 1 - (mouseState.position.x - lastMouseState.position.x + mouseState.position.y - lastMouseState.position.y) * 0.01f;
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
	}





	if (!wm->keyPressed)
	{
		if (keyState.isPressed(blib::Key::DEL) && !lastKeyState.isPressed(blib::Key::DEL))
		{
			for (int i = 0; i < (int)map->getRsw()->objects.size(); i++)
			{
				if (map->getRsw()->objects[i]->selected)
				{
					map->getRsw()->objects.erase(map->getRsw()->objects.begin() + i);
					i--;

					objectWindow->updateObjects(map);
				}
			}
		}
		if (keyState.isPressed(blib::Key::D) && !lastKeyState.isPressed(blib::Key::D))
		{
			for (int i = 0; i < (int)map->getRsw()->objects.size(); i++)
			{
				if (map->getRsw()->objects[i]->selected)
				{
					if (map->getRsw()->objects[i]->type == Rsw::Object::Type::Model)
						addModel(((Rsw::Model*)map->getRsw()->objects[i])->fileName);

					map->getRsw()->objects[map->getRsw()->objects.size() - 1]->rotation = map->getRsw()->objects[i]->rotation;
					map->getRsw()->objects[map->getRsw()->objects.size() - 1]->scale = map->getRsw()->objects[i]->scale;

					map->getRsw()->objects[i]->selected = false;
					map->getRsw()->objects[map->getRsw()->objects.size()-1]->selected = true;
					objectWindow->updateObjects(map);
					break;
				}
			}
		}
	}
}
